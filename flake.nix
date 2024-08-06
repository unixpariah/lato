{
  description = "Text rendering library";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
  };

  outputs = {
    nixpkgs,
    flake-utils,
    ...
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = import nixpkgs {
          inherit system;
        };
      in {
        devShell = pkgs.mkShell {
          buildInputs = with pkgs; [
            clang-tools
            pkg-config
            freetype
            gcc
            libGL
            fontconfig
            valgrind
            glfw
          ];
        };

        packages.default = pkgs.callPackage ./default.nix {};
      }
    );
}
