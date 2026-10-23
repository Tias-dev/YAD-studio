{pkgs, ...}: {
  packages = with pkgs;
  with pkgs.qt6; [
    qtbase
    cmake
    qttools
    wrapQtAppsHook
    qtsvg
    qtimageformats
  ];
}
