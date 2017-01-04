#include "PlayListItemVideo.h"
#include <wx/xml/xml.h>
#include <wx/notebook.h>
#include "PlayListItemVideoPanel.h"
#include "../../xLights/VideoReader.h"

PlayListItemVideo::PlayListItemVideo(wxXmlNode* node) : PlayListItem(node)
{
    _videoFile = "";
    _origin.x = 0;
    _origin.y = 0;
    _size.SetWidth(100);
    _size.SetHeight(100);
    _videoReader = nullptr;
    _durationMS = 0;
    PlayListItemVideo::Load(node);
}

void PlayListItemVideo::Load(wxXmlNode* node)
{
    PlayListItem::Load(node);
    _videoFile = node->GetAttribute("VideoFile", "");
    _origin = wxPoint(wxAtoi(node->GetAttribute("X", "0")), wxAtoi(node->GetAttribute("Y", "0")));
    _size = wxSize(wxAtoi(node->GetAttribute("W", "100")), wxAtoi(node->GetAttribute("H", "100")));
}

PlayListItemVideo::PlayListItemVideo() : PlayListItem()
{
    _videoFile = "";
    _origin.x = 0;
    _origin.y = 0;
    _durationMS = 0;
    _size.SetWidth(100);
    _size.SetHeight(100);
    _videoReader = nullptr;
}

PlayListItem* PlayListItemVideo::Copy() const
{
    PlayListItemVideo* res = new PlayListItemVideo();
    res->_origin = _origin;
    res->_size = _size;
    res->_videoFile = _videoFile;
    res->_durationMS = _durationMS;
    PlayListItem::Copy(res);

    return res;
}

wxXmlNode* PlayListItemVideo::Save()
{
    wxXmlNode * node = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, "PLIVideo");

    node->AddAttribute("VideoFile", _videoFile);
    node->AddAttribute("X", wxString::Format(wxT("%i"), _origin.x));
    node->AddAttribute("Y", wxString::Format(wxT("%i"), _origin.y));
    node->AddAttribute("W", wxString::Format(wxT("%i"), _size.GetWidth()));
    node->AddAttribute("H", wxString::Format(wxT("%i"), _size.GetHeight()));

    PlayListItem::Save(node);

    return node;
}

void PlayListItemVideo::Configure(wxNotebook* notebook)
{
    notebook->AddPage(new PlayListItemVideoPanel(notebook, this), "Video", true);
}

std::string PlayListItemVideo::GetName() const
{
    wxFileName fn(_videoFile);
    if (fn.GetName() == "")
    {
        return "Video";
    }
    else
    {
        return fn.GetName().ToStdString();
    }
}

size_t PlayListItemVideo::GetDurationMS() const
{
    return _delay + _durationMS;
}

void PlayListItemVideo::SetVideoFile(const std::string& videoFile) 
{ 
    _videoFile = videoFile; 
    OpenFiles();
    _durationMS = _videoReader->GetLengthMS();
    CloseFiles();
    _dirty = true; 
}

void PlayListItemVideo::CloseFiles()
{
    if (_videoReader != nullptr)
    {
        delete _videoReader;
        _videoReader = nullptr;
    }
}

void PlayListItemVideo::OpenFiles()
{
    CloseFiles();
    _videoReader = new VideoReader(_videoFile, _size.GetWidth(), _size.GetHeight(), false);
}

void PlayListItemVideo::Frame(wxByte* buffer, size_t size, size_t ms, size_t framems)
{
    AVFrame* img = _videoReader->GetNextFrame(ms);

    #pragma todo ... now I need to draw it
}

void PlayListItemVideo::Start()
{
    OpenFiles();
}

void PlayListItemVideo::Stop()
{
    CloseFiles();
}