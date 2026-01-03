let
  pkgs = import <nixpkgs> {};
in
pkgs.stdenv.mkDerivation rec {
  pname = "eeeeee";
  version = "1.0.1";

  src = pkgs.fetchFromGitHub {
    owner = "AlphaBril";
    repo = "Eeeeee";
    rev = "v${version}";
    sha256 = "1jq0lwzhdaa5ni8b9cprk3cfm432ipx0y1q0y808mjj5j9dspimh";
  };

  nativeBuildInputs = [ pkgs.meson pkgs.ninja pkgs.pkg-config ];
  buildInputs = [ pkgs.gtk4 pkgs.libevdev pkgs.glib ];

  meta = {
    description = "eeeeee is a GTK4-powered popup for accent characters";
    homepage = "https://github.com/AlphaBril/Eeeeee";
    license = pkgs.lib.licenses.asl20; # Apache 2.0
    maintainers = with pkgs.lib.maintainers; [ AlphaBril ];
  };
}
