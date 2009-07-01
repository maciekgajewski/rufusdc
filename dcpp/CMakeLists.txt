project( dcpp )
cmake_minimum_required(VERSION 2.6)

set(
	SOURCES

	AdcCommand.cpp
	AdcHub.cpp
	ADLSearch.cpp
	BufferedSocket.cpp
	BZUtils.cpp
	Client.cpp
	ClientManager.cpp
	ConnectionManager.cpp
	CryptoManager.cpp
	DCPlusPlus.cpp
	DirectoryListing.cpp
	Download.cpp
	DownloadManager.cpp
	Encoder.cpp
	Exception.cpp
	FavoriteManager.cpp
	File.cpp
	FinishedItem.cpp
	FinishedManager.cpp
	HashBloom.cpp
	HashManager.cpp
	HttpConnection.cpp
	LogManager.cpp
	NmdcHub.cpp
	QueueItem.cpp
	QueueManager.cpp
	ResourceManager.cpp
	SearchManager.cpp
	SearchResult.cpp
	ServerSocket.cpp
	SettingsManager.cpp
	SFVReader.cpp
	ShareManager.cpp
	SimpleXML.cpp
	Socket.cpp
	SSL.cpp
	SSLSocket.cpp
	stdinc.cpp
	StringTokenizer.cpp
	Text.cpp
	Thread.cpp
	TigerHash.cpp
	TimerManager.cpp
	Transfer.cpp
	Upload.cpp
	UploadManager.cpp
	UserConnection.cpp
	User.cpp
	Util.cpp
	version.cpp
	ZUtils.cpp
)
add_definitions( -DBUILDING_DCPP )
add_library( dcpp ${SOURCES} )

# libraries to link
target_link_libraries(
	dcpp
	
	bz2
	ssl
)

