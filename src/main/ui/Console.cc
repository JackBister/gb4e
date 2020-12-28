#include "Console.hh"

#include <deque>
#include <mutex>

#include <imgui.h>

#include "UiCommon.hh"
#include "logging/LogAppender.hh"

size_t constexpr MAX_LINES = 1024;

namespace gb4e::ui
{

std::deque<LogMessage> lines;
bool autoScroll = true;
bool scrollToBottom = false;

std::shared_ptr<LogAppender> GetConsoleAppender()
{
    class ConsoleLogAppender : public LogAppender
    {
    protected:
        virtual void AppendImpl(LogMessage const & message) final override
        {
            std::lock_guard<std::mutex> lock(guard);
            lines.push_back(message);
            if (lines.size() > MAX_LINES) {
                lines.pop_front();
            }
        }

    private:
        std::mutex guard;
    };
    return std::make_shared<ConsoleLogAppender>();
}

void DrawConsole()
{
    if (!showConsole) {
        return;
    }

    if (ImGui::Begin("Console")) {

        ImGui::BeginChild("ScrollArea", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

        for (auto line : lines) {
            auto isError = line.GetLevel() == LogLevel::ERROR || line.GetLevel() == LogLevel::SEVERE;
            if (isError) {
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.4f, 0.4f, 1.f));
            }
            ImGui::TextUnformatted(line.GetMessage().c_str());
            if (isError) {
                ImGui::PopStyleColor();
            }
        }

        if (scrollToBottom || (autoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())) {
            ImGui::SetScrollHereY(1.f);
        }
        scrollToBottom = false;

        ImGui::PopStyleVar();
        ImGui::EndChild();
    }
    ImGui::End();
}
};