#include "yuv.hpp"

/**
 * Initialize a YUV_Capture instance.  Allocates memory used for internal
 * processing.  After initialization, frames can be read using YUV_read.
 *
 * @param[in] fin   The file to read from.  If the function succeeds, the
 *                  capture will own this pointer from that point onwards.
 * @param[in] w     The frame width, in pixels.
 * @param[in] h     The frame height, in pixels.
 * @param[out] out  The instance to initialize.
 */
YUV_loader::YUV_loader(FILE *fin, size_t w, size_t h, struct YUV_Capture &out)
{
	out.fin = fin;
	out.width = w;
	out.height = h;

	out.ycrcb = Mat(Size(w,h), CV_8UC3);
	out.y = Mat(Size(w,h), CV_8UC1);
	out.cb = Mat(Size(w,h), CV_8UC1);
	out.cr = Mat(Size(w,h), CV_8UC1);
	out.cb_half = Mat(Size(w/2,h/2), CV_8UC1);
	out.cr_half = Mat(Size(w/2,h/2), CV_8UC1);
}

/**
 * Read a single frame from a previously-instantiated YUV_Capture instance.
 *
 * @param[in,out] cap   The capture to read from.  The result frame is stored
 *                      in cap-ycrcb.
 *
 * @returns YUV_OK if it's OK to keep reading.  YUV_EOF if EOF was reached and
 * no further reading is possible.  The capture should be cleaned up.
 * @see YUV_cleanup
 */
enum YUV_loader::YUV_ReturnValue YUV_loader::YUV_read(struct YUV_Capture &cap)
{
	size_t bytes_read;
	size_t npixels;

	npixels = cap.width*cap.height;
	bytes_read = fread(cap.y.data, sizeof(uint8_t), npixels, cap.fin);
	if (bytes_read == 0)
		return YUV_EOF;
	else if (bytes_read != npixels)
		return YUV_IO_ERROR;
	bytes_read = fread(cap.cb_half.data, sizeof(uint8_t), npixels/4, cap.fin);
	if (bytes_read != npixels/4)
		return YUV_IO_ERROR;

	bytes_read = fread(cap.cr_half.data, sizeof(uint8_t), npixels/4, cap.fin);
	if (bytes_read != npixels/4)
		return YUV_IO_ERROR;

	resize(cap.cb_half, cap.cb, cap.cb.size(), CV_INTER_CUBIC);
	resize(cap.cr_half, cap.cr, cap.cr.size(), CV_INTER_CUBIC);
	vector<Mat> channels;
    channels.push_back(cap.y);
    channels.push_back(cap.cr);
    channels.push_back(cap.cb);
	merge(channels, cap.ycrcb);

	return YUV_OK;
}
