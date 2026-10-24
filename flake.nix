{
  description = "Default flake-parts based setup";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-26.05";
    flake-parts.url = "github:hercules-ci/flake-parts";
    import-tree.url = "github:vic/import-tree";
    systems.url = "github:nix-systems/default";
    treefmt-nix = {
      url = "github:numtide/treefmt-nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };

    devenv = {
      url = "github:cachix/devenv";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    git-hooks = {
      url = "github:cachix/git-hooks.nix";
      inputs.nixpkgs.follows = "nixpkgs";
    };
    mini-compile-commands = {
      url = "github:danielbarter/mini_compile_commands";
      flake = false;
    };
  };

  outputs = inputs:
    inputs.flake-parts.lib.mkFlake {inherit inputs;} {
      imports = [inputs.treefmt-nix.flakeModule];
      systems = import inputs.systems;
      perSystem = {
        self',
        pkgs,
        ...
      }: {
        packages = {
          YAD-Studio = pkgs.qt6.callPackage ./default.nix {};
          default = self'.packages.YAD-Studio;
        };
        devShells = let
          mcc-env = (pkgs.callPackage inputs.mini-compile-commands {}).wrap pkgs.stdenv;
        in {
          default = inputs.devenv.lib.mkShell {
            inherit inputs pkgs;
            modules = [
              (import inputs.import-tree ./devenv)
              {stdenv = mcc-env;}
            ];
          };
        };

        treefmt = import ./treefmt.nix;
      };
    };
}
