#ifndef HAC_MAP_DOWNLOADER_HPP
#define HAC_MAP_DOWNLOADER_HPP

#include <mutex>
#include <vector>
#include <cstdlib>
#include <chrono>

/**
 * Map downloading class
 */
class HACMapDownloader {
public:
    enum DownloadStage {
        DOWNLOAD_STAGE_NOT_STARTED,
        DOWNLOAD_STAGE_STARTING,
        DOWNLOAD_STAGE_DOWNLOADING,
        DOWNLOAD_STAGE_COMPLETE,
        DOWNLOAD_STAGE_FAILED
    };

    /**
     * Begin the download
     */
    void dispatch();

    /**
     * Get the current download status
     * @return download status
     */
    DownloadStage get_status() noexcept;

    /**
     * Get the total downloaded size of the file.
     * @return file size bytes
     */
    std::size_t get_downloaded_size() noexcept;

    /**
     * Get the total size of the file. This can return 0 if the size is currently unknown.
     * @return file size bytes
     */
    std::size_t get_total_size() noexcept;

    /**
     * Get a vector holding all of the map data
     * @return pointer to a vector holding map data or nullptr if the download was not successful
     */
    const std::vector<std::byte> *get_map_data() noexcept;

    /**
     * Get whether or not the download is finished
     * @return true if the download is finished
     */
    bool is_finished() noexcept;

    /**
     * Get the download speed in kilobytes per second
     * @return download speed in kilobytes per second
     */
    std::size_t get_download_speed() noexcept;

    HACMapDownloader(const char *map, const char *output_file);
    ~HACMapDownloader();

private:
    /** Mutex for the rest of the data */
    std::mutex mutex;

    /** Map name being downloaded */
    std::string map;

    /** Temp file to save to */
    std::string output_file;

    /** Post! */
    std::string post_fields;

    /** File to write to as we download */
    std::FILE *output_file_handle;

    /** How much was downloaded so far */
    std::size_t downloaded_size = 0;

    /** How much is left to download */
    std::size_t total_size = 0;

    /** Current status of the download */
    DownloadStage status = DOWNLOAD_STAGE_NOT_STARTED;

    /**
     * Dispatch thread that does the map downloading
     * @param downloader downloader reference
     */
    static void dispatch_thread(HACMapDownloader *downloader);

    /** Buffer for holding data */
    std::vector<std::byte> buffer;

    /** Amount of the buffer used */
    std::size_t buffer_used;

    /** Clock to use */
    using Clock = std::chrono::steady_clock;

    /** Time when the download started*/
    Clock::time_point download_started;

    /** CURL handle */
    void *curl = nullptr;

    /** Callback class */
    class HACMapDownloaderCallback;
};

#endif
