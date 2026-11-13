{
  # The `zephyr-os` examples, see `examples/zephyr-os/AGENTS.md`.
  # The shell replaces the venv, `pip install` and `west sdk install` steps there; `west init -l . && west update --narrow`
  # is still needed to pull the Zephyr tree.
  perSystem = {pkgs, ...}: let
    # Zephyr's build-time Python dependencies, from `zephyr-src/scripts/requirements-base.txt`.
    # Those only used by `west flash`/`debug` and friends are left out, as Chiplab runs the ELF instead.
    python = pkgs.python313.withPackages (ps: [
      ps.west
      ps.pyelftools
      ps.pyyaml
      ps.pykwalify
      ps.jsonschema
      ps.packaging
      ps.anytree
      ps.intelhex
    ]);
  in {
    make-shells.default = {
      packages = [
        python
        pkgs.cmake
        pkgs.ninja
        pkgs.dtc
        pkgs.gperf
      ];

      # The Zephyr SDK `west sdk install` downloads is prebuilt for FHS systems, so build with the `arm-none-eabi-gcc` from
      # nixpkgs instead.
      env = {
        ZEPHYR_TOOLCHAIN_VARIANT = "gnuarmemb";
        GNUARMEMB_TOOLCHAIN_PATH = "${pkgs.gcc-arm-embedded}";
      };
    };
  };
}
