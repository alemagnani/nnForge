/*
 *  Copyright 2011-2013 Maxim Milakov
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "rectified_linear_layer_hessian_plain.h"

#include "../rectified_linear_layer.h"

namespace nnforge
{
	namespace plain
	{
		rectified_linear_layer_hessian_plain::rectified_linear_layer_hessian_plain()
		{
		}

		rectified_linear_layer_hessian_plain::~rectified_linear_layer_hessian_plain()
		{
		}

		const boost::uuids::uuid& rectified_linear_layer_hessian_plain::get_uuid() const
		{
			return rectified_linear_layer::layer_guid;
		}

		void rectified_linear_layer_hessian_plain::test(
			const_additional_buffer_smart_ptr input_buffer,
			additional_buffer_smart_ptr output_buffer,
			std::vector<additional_buffer_smart_ptr>& additional_buffers,
			plain_running_configuration_const_smart_ptr plain_config,
			const_layer_smart_ptr layer_schema,
			const_layer_data_smart_ptr data,
			const layer_configuration_specific& input_configuration_specific,
			const layer_configuration_specific& output_configuration_specific,
			unsigned int entry_count) const
		{
			const int elem_count = static_cast<int>(entry_count * input_configuration_specific.get_neuron_count());
			const std::vector<float>::const_iterator in_it = input_buffer->begin();
			const std::vector<float>::iterator out_it = output_buffer->begin();

			#pragma omp parallel for default(none) schedule(guided) num_threads(plain_config->openmp_thread_count)
			for(int i = 0; i < elem_count; ++i)
				*(out_it + i) = std::max<float>(*(in_it + i), 0.0F);
		}

		void rectified_linear_layer_hessian_plain::backprop(
			additional_buffer_smart_ptr input_errors,
			const_additional_buffer_smart_ptr output_errors,
			const_additional_buffer_smart_ptr output_neurons,
			std::vector<additional_buffer_smart_ptr>& additional_buffers,
			plain_running_configuration_const_smart_ptr plain_config,
			const_layer_smart_ptr layer_schema,
			const_layer_data_smart_ptr data,
			const layer_configuration_specific& input_configuration_specific,
			const layer_configuration_specific& output_configuration_specific,
			unsigned int entry_count) const
		{
			const int elem_count = static_cast<int>(entry_count * input_configuration_specific.get_neuron_count());
			const std::vector<float>::iterator in_err_it = input_errors->begin();
			const std::vector<float>::const_iterator out_it = output_neurons->begin();

			#pragma omp parallel for default(none) schedule(guided) num_threads(plain_config->openmp_thread_count)
			for(int i = 0; i < elem_count; ++i)
			{
				float val = *(out_it + i);
				if (val == 0.0F)
					*(in_err_it + i) = 0.0F;
			}
		}

		bool rectified_linear_layer_hessian_plain::is_in_place_backprop() const
		{
			return true;
		}
	}
}
