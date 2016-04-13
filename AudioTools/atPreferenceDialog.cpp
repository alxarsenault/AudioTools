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

#include "atPreferenceDialog.h"

#include <OpenAX/Core.h>
#include <OpenAX/OSFileSystem.h>
#include <OpenAX/Toggle.h>

#include "atSkin.hpp"
#include "PyoAudio.h"

namespace at {
namespace editor {
	PreferencePanel::PreferencePanel(const ax::Rect& rect)
		: _font(0)
		, _audio_rect(10, 10, rect.size.x - 20, 100)
	{
		_audio_label_rect = ax::Rect(_audio_rect.position, ax::Size(_audio_rect.size.x, 23));

		_midi_rect = ax::Rect(_audio_rect.GetNextPosDown(10), ax::Size(_audio_rect.size.x, 65));
		_midi_label_rect = ax::Rect(_midi_rect.position, ax::Size(_midi_rect.size.x, 23));

		// Create window.
		win = ax::Window::Create(rect);
		win->event.OnPaint = ax::WBind<ax::GC>(this, &PreferencePanel::OnPaint);

		PyoAudio* audio = PyoAudio::GetInstance();

		// Audio input device.
		auto btn_in = ax::shared<ax::DropMenuBox>(
			ax::Rect(ax::Point(_audio_label_rect.position.x + 95, _audio_label_rect.position.y + 30),
				ax::Size(175, 25)), audio->GetCurrentInputDevice(), audio->GetInputDevices());

		win->node.Add(btn_in);
		_menu_boxes[AUDIO_IN] = btn_in.get();

		// Audio output device.
		ax::Point o_pos(btn_in->GetWindow()->dimension.GetRect().GetNextPosDown(10));
		auto btn_out = ax::shared<ax::DropMenuBox>(ax::Rect(o_pos, ax::Size(175, 25)), audio->GetCurrentOutputDevice(), audio->GetOutputDevices());
		win->node.Add(btn_out);
		
		_menu_boxes[AUDIO_OUT] = btn_out.get();


		ax::StringVector midi_in_opts = {"Banane", "Patate"};
		
		// Midi input device.
		auto btn_midi_in = ax::shared<ax::DropMenuBox>(
			ax::Rect(ax::Point(_midi_label_rect.position.x + 95, _midi_label_rect.position.y + 30),
				ax::Size(175, 25)), "None", midi_in_opts);
		win->node.Add(btn_midi_in);
		_menu_boxes[MIDI_IN] = btn_midi_in.get();
	}

	PreferencePanel::~PreferencePanel()
	{
	}

	void PreferencePanel::OnButtonAudioInputDevice(const ax::Button::Msg& msg)
	{
		ax::StringVector drop_options = { "Banana", "Potato" };

		ax::DropMenu::Info menu_info;
		menu_info.normal = ax::Color(240, 240, 240);
		menu_info.hover = ax::Color(246, 246, 246);
		menu_info.font_color = ax::Color(0.0);
		menu_info.selected = ax::Color(41, 222, 255);
		menu_info.selected_hover = ax::Color(41, 226, 255);
		menu_info.selected_font_color = ax::Color(0.0);
		menu_info.contour = ax::Color(0.86);
		menu_info.separation = ax::Color(0.86);
		menu_info.up_down_arrow = ax::Color(0.35);
		menu_info.right_arrow = ax::Color(0.70);
		menu_info.item_height = 25;

		ax::DropMenu::Events drop_evts;

		ax::Size size(175, 300);

		ax::Button* sender = msg.GetSender();
		const ax::Point pos(sender->GetWindow()->dimension.GetRect().GetNextPosDown(0));

		auto menu = ax::shared<ax::DropMenu>(ax::Rect(pos, size), drop_evts, menu_info, drop_options);

		win->node.Add(menu);
	}

	void PreferencePanel::OnButtonAudioOutputDevice(const ax::Button::Msg& msg)
	{
	}
	
	bool PreferencePanel::IsMouseInDropMenu()
	{
		for(int i = 0; i < NUMBER_OF_PREF_BOX; i++) {
			if(_menu_boxes[i]->IsMouseInDropMenu()) {
				return true;
			}
		}
		return false;
	}

	void PreferencePanel::OnPaint(ax::GC gc)
	{
		const ax::Rect rect(win->dimension.GetDrawingRect());
		gc.SetColor(ax::Color(0.95));
		gc.DrawRectangle(rect);

		// Audio rectangle.
		gc.SetColor(ax::Color(0.80));
		gc.DrawRectangleContour(_audio_rect);

		// Audio label.
		gc.SetColor(ax::Color(0.80));
		gc.DrawRectangle(_audio_label_rect);

		gc.SetColor(ax::Color(0.0));
		gc.DrawStringAlignedCenter(_font, "Audio", _audio_label_rect);

		// Audio input device.
		gc.SetColor(ax::Color(0.0));
		const ax::Point in_dev_pos(_audio_label_rect.position.x + 8, _audio_label_rect.position.y + 35);
		gc.DrawString(_font, "Input device    : ", in_dev_pos);

		// Audio output device.
		const ax::Point out_dev_pos(in_dev_pos + ax::Point(0, 34));
		gc.DrawString(_font, "Output device : ", out_dev_pos);

		// Midi rectangle.
		gc.SetColor(ax::Color(0.80));
		gc.DrawRectangleContour(_midi_rect);

		// Midi label.
		gc.SetColor(ax::Color(0.80));
		gc.DrawRectangle(_midi_label_rect);

		gc.SetColor(ax::Color(0.0));
		gc.DrawStringAlignedCenter(_font, "Midi", _midi_label_rect);

		// Audio input device.
		gc.SetColor(ax::Color(0.0));
		const ax::Point midi_in_dev_pos(_midi_label_rect.position.x + 8, _midi_label_rect.position.y + 35);
		gc.DrawString(_font, "Input device    : ", midi_in_dev_pos);

		// Preference contour.
		gc.SetColor(ax::Color(0.80));
		gc.DrawRectangleContour(rect);
	}

	PreferenceDialog::PreferenceDialog(const ax::Rect& rect)
	{
		// Create window.
		win = ax::Window::Create(rect);
		win->event.OnPaint = ax::WBind<ax::GC>(this, &PreferenceDialog::OnPaint);
		win->event.OnGlobalClick
			= ax::WBind<ax::Window::Event::GlobalClick>(this, &PreferenceDialog::OnGlobalClick);
		win->event.OnMouseLeftDown = ax::WBind<ax::Point>(this, &PreferenceDialog::OnMouseLeftDown);

		ax::App::GetInstance().GetPopupManager()->AddGlobalClickListener(win);

		ax::Size pref_size(300, 194);
		ax::Point pos((rect.size.x - pref_size.x) / 2, (rect.size.y - pref_size.y) / 2);

		auto pref_panel = ax::shared<PreferencePanel>(ax::Rect(pos, pref_size));
		win->node.Add(pref_panel);
		_pref_panel = pref_panel.get();
	}

	void PreferenceDialog::OnGlobalClick(const ax::Window::Event::GlobalClick& gclick)
	{
		if(_pref_panel != nullptr) {
			if(!ax::App::GetInstance().GetPopupManager()->IsMouseStillInChildWindow(_pref_panel->GetWindow())) {
				
				if(!_pref_panel->IsMouseInDropMenu()) {
					DeleteDialog();
				}
			}
		}
	}

	void PreferenceDialog::DeleteDialog()
	{
		ax::App::GetInstance().GetWindowManager()->SetPastWindow(nullptr);
		ax::App::GetInstance().GetWindowManager()->UnGrabKey();
		ax::App::GetInstance().GetWindowManager()->UnGrabMouse();

		win->event.UnGrabKey();
		win->event.UnGrabMouse();
		win->backbone = nullptr;

		ax::App::GetInstance().GetPopupManager()->RemoveGlobalClickListener(win);
		ax::App::GetInstance().GetPopupManager()->GetWindowTree()->GetNodeVector().clear();
		ax::App::GetInstance().GetPopupManager()->UnGrabKey();
		ax::App::GetInstance().GetPopupManager()->UnGrabMouse();
		ax::App::GetInstance().GetPopupManager()->SetPastWindow(nullptr);
		ax::App::GetInstance().UpdateAll();
	}

	void PreferenceDialog::OnMouseLeftDown(const ax::Point& pos)
	{
		//	win->PushEvent(CANCEL, new ax::Event::StringMsg(""));
		DeleteDialog();
	}

	void PreferenceDialog::OnPaint(ax::GC gc)
	{
		const ax::Rect rect(win->dimension.GetDrawingRect());

		gc.SetColor(ax::Color(0.0, 0.6));
		gc.DrawRectangle(rect);
	}
}
}