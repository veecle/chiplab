{
  # The Mintlify docs site under `docs/`, see `package.json` (`npm ci && npm run docs:dev`).
  # Node 20 (`.nvmrc`) is EOL in nixpkgs, so this uses the newest major `package.json`'s `engines` allows.
  perSystem = {pkgs, ...}: {
    make-shells.default = {
      packages = [pkgs.nodejs_24];
    };
  };
}
