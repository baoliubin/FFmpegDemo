import qbs

Product {
	name:"Remuxing"
	type:"application"
	consoleApplication: false
	Depends {name:"cpp"}

	cpp.cxxLanguageVersion:"c++11"
	cpp.libraryPaths: [project.ffmpegPath + "/lib"]
	cpp.includePaths: [project.ffmpegPath + "/include"]
	cpp.dynamicLibraries:[
		"avcodec",
		"avfilter",
		"avformat",
		"avutil",
		"swresample",
		"avdevice"
	]

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


