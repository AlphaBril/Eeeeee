let
  pkgs = import <nixpkgs> {};
in
pkgs.stdenv.mkDerivation rec {
  pname = "eeeeee";
  version = "1.0.0";

  src = pkgs.fetchFromGitHub {
    owner = "AlphaBril";
    repo = "Eeeeee";
    rev = "v${version}";
    sha256 = "sha256-PLACEHOLDER";
  };

  nativeBuildInputs = [ pkgs.meson pkgs.ninja pkgs.pkg-config ];
  buildInputs = [ pkgs.gtk4 pkgs.libevdev pkgs.glib ];

  mesonFlags = [
    "-Dprefix=$out"
  ];

  installPhase = ''
    ninja install
  '';

  meta = {
    description = "eeeeee is a GTK4-powered popup for accent characters";
    homepage = "https://github.com/AlphaBril/Eeeeee";
    license = pkgs.lib.licenses.asl20; # Apache 2.0
    maintainers = with pkgs.lib.maintainers; [ AlphaBril ];
  };
}
