set(CPM_DOWNLOAD_VERSION 0.43.2) # The version of CPM to download
set(CPM_HASH_SUM "49a3bef91ceb65bb66d57255e12d1ffd22abc2f6408fa9fe4534c544a2f232aa") # The SHA256 hash of the CPM download

set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake") # The location where CPM will be downloaded to

file(DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake # The URL to download CPM from
    ${CPM_DOWNLOAD_LOCATION} # The location to download CPM to
    EXPECTED_HASH SHA256=${CPM_HASH_SUM} # The expected SHA256 hash of the downloaded file
)

include(${CPM_DOWNLOAD_LOCATION}) # Include the downloaded CPM.cmake file