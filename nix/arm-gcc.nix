{root, ...}: {
  # The `freertos` and `threadx` examples build with `arm-none-eabi-gcc` and `make`, and fetch their kernels with `git`.
  perSystem = {
    lib,
    pkgs,
    ...
  }: let
    # The kernel tag each example's `Makefile` pins (and Renovate bumps) in `KERNEL_TAG`, with only the hash kept here,
    # so a bump fails evaluation until the hash for the new tag is added.
    kernels = {
      freertos = {
        dir = "freertos-kernel";
        owner = "FreeRTOS";
        repo = "FreeRTOS-Kernel";
        hashes = {
          "V11.3.3" = "sha256-tDp8tmGlbnedguAjsQFUfRClq4OR7n2utTE+UZ8Kmmw=";
        };
      };
      threadx = {
        dir = "threadx-kernel";
        owner = "eclipse-threadx";
        repo = "threadx";
        hashes = {
          "v6.5.2.202603b_rel" = "sha256-9OO65dlVM6txA638d2BOaKyE7alf6W0r4oZO81hVrvY=";
        };
      };
    };

    boards = framework:
      lib.attrNames (lib.filterAttrs (_: type: type == "directory") (builtins.readDir (root + "/examples/${framework}")));

    buildExample = framework: board: let
      src = root + "/examples/${framework}/${board}";
      kernel = kernels.${framework};
      tag = lib.head (builtins.match ".*KERNEL_TAG[[:space:]]*:=[[:space:]]*([^[:space:]]+).*" (builtins.readFile (src + "/Makefile")));
      kernelSrc = pkgs.fetchFromGitHub {
        inherit (kernel) owner repo;
        inherit tag;
        hash = kernel.hashes.${tag} or (throw "no hash for ${kernel.repo} ${tag} in nix/arm-gcc.nix");
      };
    in
      pkgs.stdenvNoCC.mkDerivation {
        name = "hello-${board}";
        inherit src;
        nativeBuildInputs = [pkgs.gcc-arm-embedded pkgs.gnumake];
        # Stands in for `make setup`.
        postUnpack = ''
          ln -s ${kernelSrc} "$sourceRoot/${kernel.dir}"
        '';
        installPhase = ''
          runHook preInstall
          install -D build/hello-${board}.elf $out/hello-${board}.elf
          runHook postInstall
        '';
        dontFixup = true;
      };

    examples = lib.listToAttrs (lib.concatMap (framework:
      map (board: lib.nameValuePair "${framework}-${board}" (buildExample framework board)) (boards framework))
    (lib.attrNames kernels));
  in {
    make-shells.default = {
      packages = [
        pkgs.gcc-arm-embedded
        pkgs.gnumake
        pkgs.git
      ];
    };

    packages = examples;
    checks = examples;
  };
}
