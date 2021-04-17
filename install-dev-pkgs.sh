#!/bin/sh
#
# Helper script to install additional packages needed to compile
# Helium on Helium image
#

pkg install gmake autoconf automake libtool pkgconf nasm \
	cmake gperf meson intltool bison \
	talloc libmtdev gettext-tools py37-mako
