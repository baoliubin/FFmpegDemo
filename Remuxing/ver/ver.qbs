import qbs 1.0
Product {
    name:"app_version_header"
    type:"hpp"
    files:"app_version.h.in"
    Export {
        Depends{name:"cpp"}
        cpp.includePaths: product.buildDirectory
    }
}
