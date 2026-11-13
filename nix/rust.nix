{root, ...}: {
  # The `bare-metal` and `embassy-rust` examples.
  perSystem = {
    lib,
    pkgs,
    ...
  }: let
    boards = framework:
      lib.attrNames (lib.filterAttrs (_: type: type == "directory") (builtins.readDir (root + "/examples/${framework}")));

    exampleDirs =
      lib.concatMap (framework: map (board: root + "/examples/${framework}/${board}") (boards framework))
      ["bare-metal" "embassy-rust"];

    # Each example pins its target in its `.cargo/config.toml`.
    targets = lib.unique (map (dir: (lib.importTOML (dir + "/.cargo/config.toml")).build.target) exampleDirs);

    rustToolchain = pkgs.rust-bin.stable.latest.default.override {
      extensions = ["rust-src" "rust-analyzer"];
      inherit targets;
    };
  in {
    make-shells.default = {
      packages = [rustToolchain];
    };
  };
}
