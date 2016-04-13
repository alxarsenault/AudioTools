/*
 * Copyright (c) 2016 AudioTools - All Rights Reserved
 *
 * This Software may not be distributed in parts or its entirety
 * without prior written agreement by AutioTools.
 *
 * Neither the name of the AudioTools nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY AUDIOTOOLS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL AUDIOTOOLS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Written by Alexandre Arsenault <alx.arsenault@gmail.com>
 */
 
#ifndef mdiStatusBar_hpp
#define mdiStatusBar_hpp

#include <OpenAX/OpenAX.h>

#include <OpenAX/Button.h>
#include <OpenAX/Toggle.h>

#include "atVolumeMeter.hpp"

namespace at {
namespace editor {
	/*
	 * ax::editor::StatusBar.
	 */
	class StatusBar : public ax::Window::Backbone {
	public:
		StatusBar(const ax::Rect& rect);

		enum : ax::Event::Id {
			SAVE_LAYOUT,
			SAVE_AS_LAYOUT,
			OPEN_LAYOUT,
			VIEW_LAYOUT,
			RELOAD_SCRIPT,
			TOGGLE_LEFT_PANEL,
			TOGGLE_BOTTOM_PANEL,
			TOGGLE_RIGHT_PANEL
		};
		
		void SetLayoutFilePath(const std::string& path)
		{
			_layout_file_path = path;
			win->Update();
		}

	private:
		ax::Font _font;
		ax::Toggle* _toggle_left;
		ax::Toggle* _toggle_bottom;
		ax::Toggle* _toggle_right;
		at::VolumeMeter* _volumeMeterLeft;
		at::VolumeMeter* _volumeMeterRight;
		std::string _layout_file_path;
		
		axEVENT_DECLARATION(ax::Button::Msg, OnSaveLayout);
		axEVENT_DECLARATION(ax::Button::Msg, OnSaveAsLayout);
		axEVENT_DECLARATION(ax::Button::Msg, OnOpenLayout);
		axEVENT_DECLARATION(ax::Button::Msg, OnViewLayout);
		axEVENT_DECLARATION(ax::Button::Msg, OnReload);
		axEVENT_DECLARATION(ax::Button::Msg, OnSettings);

		axEVENT_DECLARATION(ax::Toggle::Msg, OnToggleLeftPanel);
		axEVENT_DECLARATION(ax::Toggle::Msg, OnToggleBottomPanel);
		axEVENT_DECLARATION(ax::Toggle::Msg, OnToggleRightPanel);

		axEVENT_DECLARATION(ax::Event::StringMsg, OnSaveDialog);
		axEVENT_DECLARATION(ax::Event::StringMsg, OnCancelDialog);
		
		axEVENT_DECLARATION(ax::Event::StringMsg, OnOpenDialog);
		
		typedef std::pair<double, double> StereoRmsValue;
		
		axEVENT_DECLARATION(ax::Event::SimpleMsg<StereoRmsValue>, OnAudioRmsValue);

		void OnResize(const ax::Size& size);
		void OnPaint(ax::GC gc);
	};
}
}

#endif /* mdiStatusBar_hpp */
