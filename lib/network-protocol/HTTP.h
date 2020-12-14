#ifndef NETWORKPROTOCOLHTTP_H
#define NETWORKPROTOCOLHTTP_H

#include "FS.h"
#include "WebDAV.h"
#include "../http/fnHttpClient.h"

class NetworkProtocolHTTP : public NetworkProtocolFS
{
public:
    /**
     * @brief ctor
     * @param rx_buf pointer to receive buffer
     * @param tx_buf pointer to transmit buffer
     * @param sp_buf pointer to special buffer
     * @return a NetworkProtocolFS object
     */
    NetworkProtocolHTTP(string *rx_buf, string *tx_buf, string *sp_buf);

    /**
     * dTOR
     */
    virtual ~NetworkProtocolHTTP();

    /**
     * @brief Return a DSTATS byte for a requested COMMAND byte.
     * @param cmd The Command (0x00-0xFF) for which DSTATS is requested.
     * @return a 0x00 = No payload, 0x40 = Payload to Atari, 0x80 = Payload to FujiNet, 0xFF = Command not supported.
     */
    virtual uint8_t special_inquiry(uint8_t cmd);

    /**
     * @brief execute a command that returns no payload
     * @param cmdFrame a pointer to the passed in command frame for aux1/aux2/etc
     * @return error flag. TRUE on error, FALSE on success.
     */
    virtual bool special_00(cmdFrame_t *cmdFrame);

    /**
     * @brief execute a command that returns a payload to the atari.
     * @param sp_buf a pointer to the special buffer
     * @param len Length of data to request from protocol. Should not be larger than buffer.
     * @return error flag. TRUE on error, FALSE on success.
     */
    virtual bool special_40(uint8_t *sp_buf, unsigned short len, cmdFrame_t *cmdFrame);

    /**
     * @brief execute a command that sends a payload to fujinet (most common, XIO)
     * @param sp_buf, a pointer to the special buffer, usually a EOL terminated devicespec.
     * @param len length of the special buffer, typically SPECIAL_BUFFER_SIZE
     */
    virtual bool special_80(uint8_t *sp_buf, unsigned short len, cmdFrame_t *cmdFrame);

    /**
     * @brief Rename file specified by incoming devicespec.
     * @param url pointer to EdUrlParser pointing to file/dest to rename
     * @param cmdFrame the command frame
     * @return TRUE on error, FALSE on success
     */
    virtual bool rename(EdUrlParser *url, cmdFrame_t *cmdFrame);

    /**
     * @brief Delete file specified by incoming devicespec.
     * @param url pointer to EdUrlParser pointing to file to delete
     * @param cmdFrame the command frame
     * @return TRUE on error, FALSE on success
     */
    virtual bool del(EdUrlParser *url, cmdFrame_t *cmdFrame);

    /**
     * @brief Make directory specified by incoming devicespec.
     * @param url pointer to EdUrlParser pointing to file to delete
     * @param cmdFrame the command frame
     * @return TRUE on error, FALSE on success
     */
    virtual bool mkdir(EdUrlParser *url, cmdFrame_t *cmdFrame);

    /**
     * @brief Remove directory specified by incoming devicespec.
     * @param url pointer to EdUrlParser pointing to file to delete
     * @param cmdFrame the command frame
     * @return TRUE on error, FALSE on success
     */
    virtual bool rmdir(EdUrlParser *url, cmdFrame_t *cmdFrame);

    /**
     * @brief lock file specified by incoming devicespec.
     * @param url pointer to EdUrlParser pointing to file to delete
     * @param cmdFrame the command frame
     * @return TRUE on error, FALSE on success
     */
    virtual bool lock(EdUrlParser *url, cmdFrame_t *cmdFrame);

    /**
     * @brief unlock file specified by incoming devicespec.
     * @param url pointer to EdUrlParser pointing to file to delete
     * @param cmdFrame the command frame
     * @return TRUE on error, FALSE on success
     */
    virtual bool unlock(EdUrlParser *url, cmdFrame_t *cmdFrame);

protected:
    /**
     * Is rename implemented?
     */
    bool rename_implemented = true;

    /**
     * Is delete implemented?
     */
    bool delete_implemented = true;

    /**
     * Is mkdir implemented?
     */
    bool mkdir_implemented = true;

    /**
     * Is rmdir implemented?
     */
    bool rmdir_implemented = true;

    /**
     * @brief Open file handle, set fd
     * @return FALSE if successful, TRUE on error.
     */
    virtual bool open_file_handle();

    /**
     * @brief Open directory handle
     * @return FALSE if successful, TRUE on error.
     */
    virtual bool open_dir_handle();

    /**
     * @brief Do HTTP mount
     * @param hostName - host name of HTTP server
     * @param path - path to mount, usually "/"
     * @return false on no error, true on error.
     */
    virtual bool mount(string hostName, string path);

    /**
     * @brief Unmount HTTP server specified in mountInfo.
     * @return  false on no error, true on error.
     */
    virtual bool umount();

    /**
     * @brief Translate filesystem error codes to Atari error codes. Sets error in Protocol.
     */
    virtual void fserror_to_error();

    /**
     * @brief Read from file handle
     * @param buf target buffer
     * @param len the number of bytes requested
     * @return FALSE if success, TRUE if error
     */
    virtual bool read_file_handle(uint8_t *buf, unsigned short len);

    /**
     * @brief read next directory entry.
     * @param buf the target buffer
     * @param len length of target buffer
     */
    virtual bool read_dir_entry(char *buf, unsigned short len);

    /**
     * @brief for len requested, break up into number of required
     *        HTTP_write() blocks.
     * @param len Requested # of bytes.
     * @return TRUE on error, FALSE on success.
     */
    virtual bool write_file_handle(uint8_t *buf, unsigned short len);

    /**
     * @brief close file handle
     * @return FALSE if successful, TRUE on error.
     */
    virtual bool close_file_handle();

    /**
     * @brief Close directory handle
     * @return FALSE if successful, TRUE on error.
     */
    virtual bool close_dir_handle();

    /**
     * @brief get status of file, filling in filesize. mount() must have already been called.
     * @param path the full path of file to resolve.
     * @return resolved path.
     */
    virtual bool stat(string path);

private:
    /**
     * DAV Handler used by protocol
     */
    WebDAV dav;

    /**
     * The HTTP verb to use when we need to do the HTTP transaction
     */
    typedef enum _HTTPMode
    {
        GET,
        POST,
        PUT,
        PROPFIND
    } HTTPMode;

    /**
     * The HTTP mode specified at open
     */
    HTTPMode httpMode;

    /**
     * The Protocol mode.
     */
    typedef enum _protocolMode
    {
        DATA,
        HEADERS
    } ProtocolMode;

    /**
     * The Protocol mode changed via XIO
     */
    ProtocolMode protocolMode;

    /**
     * FILE pointer for PUT file
     */
    ::FILE* fpPUT;

    /**
     * Name of temp file
     */
    char cPUT[32];

    /**
     * The fnHTTPClient
     */
    fnHttpClient client;

    /**
     * The HTTP result code from the last verb
     */
    int resultCode;

    /**
     * Did we attempt HTTP verb?
     */
    bool verbCompleted=false;

    /**
     * @brief parse the string of XML data from PROPFIND.
     * @param s the input string
     * @return FALSE if error, TRUE if successful
     */
    bool parse_dir(string s);

    /**
     * @brief fix the scheme from uppercase to lowercase
     */
    void fix_scheme();

    /**
     * @brief read file handle while in DATA ProtocolMode
     * @param buf pointer to target buffer
     * @param len requested length of target buffer.
     * @return FALSE if len == num of bytes read, TRUE if len != num of bytes read
     */
    bool read_file_handle_data(uint8_t* buf, unsigned short len);

    /**
     * @brief read from active http client socket. HTTP verb must have already started.
     * @param buf pointer to target buffer
     * @param len number of requested bytes
     * @return FALSE if len == num of bytes read, TRUE of len != num of bytes read
     */
    bool read_response(uint8_t* buf, unsigned short len);

};

#endif /* NETWORKPROTOCOLHTTP_H */