{pkgs ? import <nixpkgs> {}}:
pkgs.mkShell {
  nativeBuildInputs = with pkgs; [
    cmake
    pkg-config
    hyprwayland-scanner
    wayland-scanner
  ];

  buildInputs = with pkgs; [
    cairo
    libdrm
    libGL
    libxkbcommon
    libgbm
    hyprgraphics
    hyprlang
    hyprutils
    pam
    pango
    sdbus-cpp_2
    systemdLibs
    wayland
    wayland-protocols
  ];
}
