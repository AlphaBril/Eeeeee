{
  description = "Accent character selector popup";

  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = { self, nixpkgs, flake-utils }:
    flake-utils.lib.eachDefaultSystem (system:
      let
        pkgs = nixpkgs.legacyPackages.${system};
      in
      {
        packages.default = pkgs.stdenv.mkDerivation rec {
          pname = "Eeeeee";
          version = "1.0.0";

          src = ./.;

          nativeBuildInputs = with pkgs; [
            meson
            ninja
            pkg-config
          ];

          buildInputs = with pkgs; [
            gtk4
            glib
            libevdev
          ];

          meta = with pkgs.lib; {
            description = "Accent character selector popup";
            longDescription = ''
              A GTK4 application that shows a popup with accent characters
              when holding vowel keys. Allows quick selection of accented
              characters using keyboard shortcuts or mouse clicks.
            '';
            homepage = "https://github.com/AlphaBril/Eeeeee";
            license = licenses.gpl3Plus;
            maintainers = [ maintainers.yourusername ];
            platforms = platforms.linux;
          };
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            pkg-config
            gtk4
            gtk4.dev
            glib
            glib.dev
            dbus
            libevdev
            meson
            ninja
          ];
        };
      });
}