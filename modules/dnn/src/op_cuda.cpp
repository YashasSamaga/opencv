// This file is part of OpenCV project.
// It is subject to the license terms in the LICENSE file found in the top-level directory
// of this distribution and at http://opencv.org/license.html.

#include "precomp.hpp"
#include "op_cuda.hpp"

#ifdef HAVE_CUDA
using namespace cv::dnn::cuda4dnn;
#endif

namespace cv {
    namespace dnn {
#ifdef HAVE_CUDA
        CUDABackendWrapperFP32::CUDABackendWrapperFP32(Mat& m)
            : BackendWrapper(DNN_BACKEND_OPENCV, DNN_TARGET_CUDA_FP32), host(m)
        {
            CV_Assert(m.isContinuous());
            CV_Assert(m.type() == CV_32F);
            CV_Assert(m.size.dims() <= tensor_type::rank);

            shared_block = std::make_shared<shared_block_type>();
            shared_block->host_dirty = true;
            shared_block->device_dirty = false;
            shared_block->memGuard = cuda4dnn::csl::MemoryLockGuard(m.data, m.total() * sizeof(float));
            shared_block->parent = createTensorHeaderFromMat<tensor_type>(host);

            span = tensor_span_type(shared_block->parent);
        }

        CUDABackendWrapperFP32::CUDABackendWrapperFP32(const Ptr<BackendWrapper>& base_, const MatShape& shape)
            : BackendWrapper(DNN_BACKEND_OPENCV, DNN_TARGET_CUDA_FP32)
        {
            const Ptr<CUDABackendWrapperFP32> base = base_.dynamicCast<CUDABackendWrapperFP32>();
            host = base->host;

            shared_block = base->shared_block;
            span = base->span.subspan(0, std::begin(shape), std::end(shape));
        }

        Ptr<BackendWrapper> CUDABackendWrapperFP32::create(Mat& m)
        {
            return Ptr<BackendWrapper>(new CUDABackendWrapperFP32(m));
        }

        Ptr<BackendWrapper> CUDABackendWrapperFP32::create(const Ptr<BackendWrapper>& base, const MatShape& shape)
        {
            return Ptr<BackendWrapper>(new CUDABackendWrapperFP32(base, shape));
        }

        void CUDABackendWrapperFP32::copyToHost() {
            if(shared_block->device_dirty) {
                shared_block->device_dirty = false;
                copyTensorToMat(host, span, shared_block->stream);
            }
        }

        void CUDABackendWrapperFP32::setHostDirty() {
            shared_block->device_dirty = false;
            shared_block->host_dirty = true;
        }

        void CUDABackendWrapperFP32::copyToDevice() {
            if(shared_block->host_dirty) {
                shared_block->host_dirty = false;
                copyMatToTensor(span, host, shared_block->stream);
            }
        }

        void CUDABackendWrapperFP32::setDeviceDirty() {
            shared_block->device_dirty = true;
            shared_block->host_dirty = false;
        }

        void CUDABackendWrapperFP32::setStream(cuda4dnn::csl::Stream stream) noexcept {
            shared_block->stream = std::move(stream);
        }

        CUDABackendWrapperFP32::tensor_span_type& CUDABackendWrapperFP32::getSpan() noexcept {
            shared_block->device_dirty = true;
            return span;
        }

        CUDABackendWrapperFP32::tensor_view_type CUDABackendWrapperFP32::getView() noexcept {
            copyToDevice();
            return static_cast<tensor_view_type>(span);
        }

#endif
    } /* namespace dnn */
} /* namespace cv */
