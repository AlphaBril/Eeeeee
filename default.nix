let
  pkgs = import <nixpkgs> {};
in
pkgs.stdenv.mkDerivation {
  pname = "eeeeee";
  version = "1.0.2";

  src = ./.;

  nativeBuildInputs = [ pkgs.meson pkgs.ninja pkgs.pkg-config ];
  buildInputs = [ pkgs.gtk4 pkgs.libevdev pkgs.glib ];

  meta = {
    description = "eeeeee is a GTK4-powered popup for accent characters";
    homepage = "https://github.com/AlphaBril/Eeeeee";
    license = pkgs.lib.licenses.asl20; # Apache 2.0
    platforms = pkgs.lib.platforms.linux;
  };
}
