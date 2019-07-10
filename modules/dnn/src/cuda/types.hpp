// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#ifndef OPENCV_DNN_SRC_CUDA_TYPE_HPP
#define OPENCV_DNN_SRC_CUDA_TYPE_HPP

#include <cstdint>

namespace cv { namespace dnn { namespace cuda4dnn { namespace csl {

    /* For indices, we can use 32bit variables or 64bit variables. The GPU registers are 32 bits in size.
     * Hence, a 64bit variable requires two registers and is slightly slower than the 32bit versions for some
     * operations.
     */
    namespace gpu {
#ifdef __CUDACC__
    using size_type = int;
    using index_type = int;
#else
    using size_type = std::int32_t;
    using index_type = std::int32_t;
#endif
    }

}}}} /*  cv::dnn::cuda4dnn::csl */

#endif /* OPENCV_DNN_SRC_CUDA_TYPE_HPP */
