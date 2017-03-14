import qbs

Project {
	property string ffmpegPath : {
		if (qbs.hostOS.contains("unix") || qbs.hostOS.contains("linux"))
			return "/usr/local/ffmpeg/shared"
		else if (qbs.hostOS.contains("windows"))
			return sourceDirectory + "/FFMPEG"
	}

    references: [
        "Remuxing/Remuxing.qbs"
    ]
}
