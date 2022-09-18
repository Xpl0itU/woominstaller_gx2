FROM wiiuenv/devkitppc:20220806 AS final

CMD dkp-pacman -Syyu --noconfirm ppc-freetype

WORKDIR /project
