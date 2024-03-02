{ pkgs ? import <nixpkgs> {}, stdenv ? pkgs.stdenv }:

stdenv.mkDerivation rec {
  pname = "powermenu";
  version = "1.0";
  
  src = fetchGit {
    url = "https://github.com/ozpv/powermenu.git";
    rev = "ea722a33a5041756618f1e1d312a08c3bfd14add";
  };

  makeFlags = [ "PREFIX=$(out)" "CC=${stdenv.cc.targetPrefix}cc" ];
}
