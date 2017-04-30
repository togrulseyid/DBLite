#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <dblite.h>

/**
 * \class Operations
 *
 * \brief Helper class for file, conversion  operations.
 *
 * This class has functionality to write/read file, read as chunks and ect.
 *
 * \note Some functionalities including <b>writeInt64</b> has not been implemented
 *
 * ### Usage
 * Operations ops;<br />
 * std::string fileContent = ops.readFile("MY_GORGEOUS_FILE_NAME"); <br />
 *
 */
class Operations {
public:

    /*!
     * \brief Write array of int64_t to the file
     *
     * Writes vector<int64_t> data to given path in given openmode format
     *
     * ### Usage
     * ops.#writeInt64(<b>data_path.bin</b>,
     * <b>std::ios::app | std::ios::binary</b>, <b>*data</b>);
     *
     * @param[out] bool if data written to the file or not
     * @param[in] fileName
     *  \parblock
     *  std::string Name of file
     *  \endparblock
     * @param[in]  base
     *  \parblock
     *    std::ios_base::openmode for modes of writing to the file.
     *    <b>Example: </b>std::ios::app | std::ios::binary
     *  \endparblock
     * @param[in]  data int64_t array
     *
     * \return true if it was successful to write to file, else false
     */
    bool writeInt64(std::string fileName, std::ios_base::openmode base, std::vector<int64_t> data);

    /*!
     * \brief Returns size of file in bytes
     *
     * @param[out] file size
     * @param[in] fileName
     *  \parblock
     *  std::string Name of file
     *  \endparblock
     */
    uint64_t getFileSize(std::string fileName);

    /*!
     * \brief Write string data to file
     *
     * @param[out] bool if written
     * @param[in] fileName
     *  \parblock
     *  std::string Name of file
     *  \endparblock
     * @param[in]  base
     *  \parblock
     *    std::ios_base::openmode for modes of writing to the file.
     *    <b>Example: </b>std::ios::app | std::ios::binary
     *  \endparblock
     * @param[in]  data std::string
     */
    bool writeToFile(std::string fileName, std::ios_base::openmode base, std::string data);

    /*!
     * \brief Read a file as string
     *
     * @param[out] string file content
     * @param[in] fileName
     *  \parblock
     *  std::string Name of file
     *  \endparblock
     */
    std::string readFile(std::string fileName);

}; // end of class Operations

#endif
