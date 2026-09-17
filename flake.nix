{
  description = "flake for stacer";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs?ref=nixos-unstable";
  };

  outputs = { self, nixpkgs }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];

      forAllSystems = nixpkgs.lib.genAttrs systems;
    in {
      packages = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
        in {
          default = pkgs.stdenv.mkDerivation {
            pname = "stacer";
            version = "1.8.0";

            src = ./.;

            nativeBuildInputs = with pkgs; [
              cmake
              qt6.qttools
              qt6.wrapQtAppsHook
            ];

            buildInputs = with pkgs; [
              qt6.qtbase
              qt6.qtcharts
            ];

            cmakeBuildType = "Debug";

            meta = {
              description = "Linux System Optimizer and Monitoring";
              mainProgram = "stacer";
              platforms = pkgs.lib.platforms.linux;
            };
          };
        });

      apps = forAllSystems (system: {
        default = {
          type = "app";
          program = "${self.packages.${system}.default}/bin/stacer";
        };
      });

      devShells = forAllSystems (system:
        let
          pkgs = import nixpkgs { inherit system; };
        in {
          default = pkgs.mkShell {
            packages = with pkgs; [
              cmake
              qt6.qttools
              qt6.qtbase
              qt6.qtcharts
            ];
          };
        });
    };
}
