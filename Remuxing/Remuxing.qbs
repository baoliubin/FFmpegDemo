import qbs

Project{

    references: ["ver/ver.qbs"]

    Product {
        name:"Remuxing"
        type:"application"
        consoleApplication: false
        Depends {name:"app_version_header"}
        Depends {name:"cpp"}

        property string ffmpegPath : {
            if (qbs.hostOS.contains("unix") || qbs.hostOS.contains("linux"))
                return "/usr/local/ffmpeg/shared"
            else if (qbs.hostOS.contains("windows"))
                return "F:/ffmepg/ffmpeg"
        }

        cpp.cxxLanguageVersion:"c++11"
        cpp.defines: ["DEFINE_TEST", "SHOW_LOG", "USE_COLORS=5", 'COLOR_STR="blanched almond"']
        cpp.includePaths: [ffmpegPath + "/include"]
        cpp.libraryPaths: [ffmpegPath + "/lib"]

        cpp.dynamicLibraries:[
            "avcodec",
            "avfilter",
            "avformat",
            "avutil",
            "swresample",
        ]

        cpp.prefixHeaders:["src/copyright.h"]
        cpp.optimization:"ludicrousSpeed"

        Depends{
            name:"Qt"
            submodules: ["core", "gui", "widgets"]
        }
        Group {
            files: [
                "src/args.cpp",
                "src/args.h",
                "src/ffmpegHeader.cpp",
                "src/ffmpegHeader.h",
                "src/main.cpp",
            ]
            name:"Sources"
        }
        Group {
            name:"Resources"
        }

        Group {
            fileTagsFilter: product.type
            qbs.install:true
            qbs.installDir:"install-root"
        }
    }

}
