{ pkgs ? import <nixpkgs> {} }:

pkgs.mkShell {
  buildInputs = [
    pkgs.pkg-config
    
    pkgs.freeglut
    pkgs.libGL
    pkgs.libGLU

    pkgs.freetype

    pkgs.libpng
    pkgs.libjpeg
  ];
}
