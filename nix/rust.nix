{root, ...}: {
  # The `bare-metal` and `embassy-rust` examples.
  perSystem = {
    lib,
    pkgs,
    ...
  }: let
    boards = framework:
      lib.attrNames (lib.filterAttrs (_: type: type == "directory") (builtins.readDir (root + "/examples/${framework}")));

    examples = lib.concatMap (framework:
      map (board: {
        inherit framework board;
        src = root + "/examples/${framework}/${board}";
      }) (boards framework))
    ["bare-metal" "embassy-rust"];

    # Each example pins its target in its `.cargo/config.toml`.
    exampleTarget = src: (lib.importTOML (src + "/.cargo/config.toml")).build.target;

    # The same toolchain rustup users get, so Nix builds test what the docs tell them to use.
    toolchainFile = (lib.importTOML (root + "/rust-toolchain.toml")).toolchain;
    missingTargets = lib.subtractLists toolchainFile.targets (map (example: exampleTarget example.src) examples);
    targets =
      lib.throwIfNot (missingTargets == [])
      "rust-toolchain.toml is missing example targets: ${toString missingTargets}"
      toolchainFile.targets;
    rustBin = pkgs.rust-bin.stable.${toolchainFile.channel};

    rustToolchain = rustBin.default.override {
      extensions = ["rust-src" "rust-analyzer"];
      inherit targets;
    };

    # Without the shell's extensions, so building the examples doesn't pull in `rust-analyzer`.
    rustPlatform = let
      toolchain = rustBin.minimal.override {inherit targets;};
    in
      pkgs.makeRustPlatform {
        cargo = toolchain;
        rustc = toolchain;
      };

    buildExample = src: let
      cargoToml = lib.importTOML (src + "/Cargo.toml");
      target = exampleTarget src;
      name = cargoToml.package.name;
    in
      rustPlatform.buildRustPackage {
        pname = name;
        inherit (cargoToml.package) version;
        inherit src;
        # Each crate is fetched as its own derivation, so ones shared between examples are only downloaded once.
        cargoLock.lockFile = src + "/Cargo.lock";
        # `cargo-auditable` passes a `-Wl,` flag `rust-lld` doesn't understand.
        auditable = false;

        # `buildRustPackage`'s hooks build for the host platform, while the example builds for the target its
        # `.cargo/config.toml` sets.
        buildPhase = ''
          runHook preBuild
          cargo build --release --frozen
          runHook postBuild
        '';
        doCheck = false;
        installPhase = ''
          runHook preInstall
          install -D target/${target}/release/${name} $out/${name}.elf
          runHook postInstall
        '';
        dontFixup = true;
      };

    packages = lib.listToAttrs (map (example:
      lib.nameValuePair "${example.framework}-${example.board}" (buildExample example.src))
    examples);
  in {
    make-shells.default = {
      packages = [rustToolchain];
    };

    inherit packages;
    checks = packages;
  };
}
