{
  # The `freertos` and `threadx` examples build with `arm-none-eabi-gcc` and `make`, and fetch their kernels with `git`.
  perSystem = {pkgs, ...}: {
    make-shells.default = {
      packages = [
        pkgs.gcc-arm-embedded
        pkgs.gnumake
        pkgs.git
      ];
    };
  };
}
