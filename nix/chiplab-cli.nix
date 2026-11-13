{
  # For uploading and running the built examples from the shell.
  perSystem = {inputs', ...}: {
    make-shells.default = {
      packages = [inputs'.chiplab-cli.packages.default];
    };
  };
}
