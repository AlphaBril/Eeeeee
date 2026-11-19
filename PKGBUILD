# Maintainer: AlphaBril <alphabril@koodibril.com>

pkgname=eeeeee
pkgver=1.1.0
pkgrel=1
pkgdesc="eeeeee is a GTK4-powered popup for accent characters"
arch=('x86_64')
url="https://github.com/AlphaBril/Eeeeee"
license=('Apache-2.0')
depends=('gtk4' 'libevdev' 'glib2')
makedepends=('meson' 'ninja' 'gcc' 'pkgconf')
source=("$pkgname-$pkgver.tar.gz::$url/archive/refs/tags/v$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
  arch-meson "Eeeeee-$pkgver" build
  meson compile -C build
}

package() {
  meson install -C build --destdir "$pkgdir"
}
