#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <mutex>

#include <vgm/audio/AudioStream.h>
#include <vgm/emu/Resampler.h>

//! Abstract class for audio stream control
class Audio_Stream
{
	public:
		inline Audio_Stream()
			: finished(false)
		{}

		inline virtual ~Audio_Stream()
		{}

		//! called by Audio_Manager when starting the stream.
		/*!
		 *  setup your resamplers and stuff here.
		 */
		virtual void setup_stream(uint32_t sample_rate) = 0;

		//! called by Audio_Manager during stream update.
		/*!
		 *  return zero to indicate that the stream should be stopped.
		 */
		virtual int get_sample(WAVE_32BS* output, int count, int channels) = 0;

		//! called by Audio_Manager when stopping the stream.
		/*!
		 *  resamplers should be cleaned up, but the playback may start again
		 *  so the "finished" state should not be updated here.
		 */
		virtual void stop_stream() = 0;

		//! get the "finished" flag status
		/*!
		 *  when set, the audio manager will stop mixing this stream and
		 *  destroy its pointer.
		 */
		inline void set_finished(bool flag)
		{
			finished = true;
		}

		//! get the "finished" flag status
		/*!
		 *  when set, the audio manager will stop mixing this stream and
		 *  destroy its pointer.
		 */
		inline bool get_finished()
		{
			return finished;
		}

	protected:
		bool finished;
};

//! Audio manager class
/*!
 *  Please note that initialization/deinitialization of the libvgm audio library 
 *  (Audio_Init() and Audio_Deinit() should be done in the main function first.
 */
class Audio_Manager
{
	public:
		// singleton guard
		Audio_Manager(Audio_Manager const&) = delete;
		void operator=(Audio_Manager const&) = delete;

		static Audio_Manager& get();

		void set_audio_enabled(bool status);
		bool get_audio_enabled() const;

		void set_window_handle(void* new_handle);

		int set_sample_rate(uint32_t new_sample_rate);
		void set_volume(float new_volume);
		float get_volume() const;

		int add_stream(std::shared_ptr<Audio_Stream> stream);

		void clean_up();

	private:
		Audio_Manager();

		void enumerate_drivers();
		void enumerate_devices();

		int open_driver();
		void close_driver();

		int open_device();
		void close_device();

		static int16_t clip16(int32_t input);

		static uint32_t callback(void* drv_struct, void* user_param, uint32_t buf_size, void* data);

		bool audio_enabled;

		int driver_id;
		int device_id;
		uint32_t sample_rate;
		uint32_t sample_size;

		float volume;
		int32_t converted_volume;
		std::vector<std::shared_ptr<Audio_Stream>> streams;

		void* window_handle;
		void* driver_handle;

		bool waiting_for_handle;
		bool driver_opened;
		bool device_opened;

		std::map<int, std::string> driver_names;
		std::map<int, std::string> device_names;

		std::mutex mutex;
};

#endif
