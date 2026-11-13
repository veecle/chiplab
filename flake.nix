{
  description = "Chiplab examples";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-26.05-small";

    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };

    make-shell = {
      url = "github:nicknovitski/make-shell";
      inputs.flake-compat.follows = "";
    };

    rust-overlay = {
      url = "github:oxalica/rust-overlay";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    chiplab-cli = {
      url = "github:veecle/chiplab-cli/v0.1.1";
      inputs = {
        nixpkgs.follows = "nixpkgs";
        flake-parts.follows = "flake-parts";
        make-shell.follows = "make-shell";
        rust-overlay.follows = "rust-overlay";
      };
    };
  };

  outputs = inputs @ {
    nixpkgs,
    flake-parts,
    make-shell,
    rust-overlay,
    ...
  }:
    flake-parts.lib.mkFlake {inherit inputs;} {
      imports = [
        make-shell.flakeModules.default
        ./nix/alejandra.nix
        ./nix/arm-gcc.nix
        ./nix/chiplab-cli.nix
        ./nix/docs.nix
        ./nix/rust.nix
        ./nix/zephyr.nix
      ];

      # Instead of each module having to use some number of `../`'s to reference the repository root,
      # pass down a nix path they can construct sources from.
      _module.args.root = ./.;

      systems = [
        "x86_64-linux"
        "aarch64-linux"
        "x86_64-darwin"
        "aarch64-darwin"
      ];

      perSystem = {system, ...}: let
        pkgs = import nixpkgs {
          inherit system;
          overlays = [
            # Provides a `rustc` with the embedded targets the examples build for, which nixpkgs' own doesn't ship.
            rust-overlay.overlays.default
          ];
        };
      in {
        _module.args.pkgs = pkgs;
      };
    };
}
