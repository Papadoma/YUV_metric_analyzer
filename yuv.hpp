#ifndef YUV_HPP_
#define YUV_HPP_

#include <stdio.h>
#include <stdint.h>
#include <opencv2/opencv.hpp>

using namespace cv;
class YUV_loader{


public:


	/**
	 * Is returned by the public API functions.
	 */
	enum YUV_ReturnValue
	{
		YUV_OK = 0,             /**< Function terminated correctly >*/
		YUV_PARAMETER_ERROR,    /**< The parameters passed to the function were
                                 incorrect */
		YUV_OUT_OF_MEMORY,      /**< The function ran out of memory >*/
		YUV_IO_ERROR,           /**< The function encountered an error reading
                                 data from disk, or reached EOF earlier than
                                 was expected (premature end of frame) >*/
		YUV_EOF                 /**< The function reached EOF (not premature) >*/
	};

	/**
	 * Used to capture YUV frames from a file on disk.
	 *
	 * @see YUV_init
	 * @see YUV_read
	 */
	struct YUV_Capture
	{
		FILE *fin;              /**< The input file pointer >*/
		size_t width;           /**< The width of the frame, in pixels >*/
		size_t height;          /**< The height of the frame, in pixels >*/

		Mat y;            /**< Used internally. >*/
		Mat cb;           /**< Used internally. >*/
		Mat cr;           /**< Used internally. >*/
		Mat cb_half;      /**< Used internally. >*/
		Mat cr_half;      /**< Used internally. >*/
		Mat ycrcb;        /**< The most-recently image (width x height, 24
                                 bit).  Stored in YCrCb order. >*/
	};

	YUV_loader(FILE *fin, size_t w, size_t h, struct YUV_Capture &out);
	enum YUV_ReturnValue YUV_read(struct YUV_Capture &cap);
};
#endif
