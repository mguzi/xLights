#ifndef XLIGHTSXMLFILE_H
#define XLIGHTSXMLFILE_H

#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <wx/textctrl.h>
#include "sequencer/SequenceElements.h"
#include "DataLayer.h"
#include "AudioManager.h"

class SequenceElements;  // forward declaration needed due to circular dependency
class xLightsFrame;


WX_DECLARE_STRING_HASH_MAP( int, StringIntMap );

class xLightsXmlFile : public wxFileName
{
    public:
        //xLightsXmlFile();
        xLightsXmlFile(const wxFileName &filename);
        virtual ~xLightsXmlFile();

        enum HEADER_INFO_TYPES
        {
            AUTHOR,
            AUTHOR_EMAIL,
            WEBSITE,
            SONG,
            ARTIST,
            ALBUM,
            URL,
            COMMENT,
            NUM_TYPES
        };

        const wxString HEADER_STRINGS[NUM_TYPES] =
        {
            "author",
            "author-email",
            "author-website",
            "song",
            "artist",
            "album",
            "MusicURL",
            "comment"
        };

        static const wxString ERASE_MODE;
        static const wxString CANVAS_MODE;

        bool Open();

        void Save( SequenceElements& elements);
        wxXmlDocument& GetXmlDocument() { return seqDocument; }
        DataLayerSet& GetDataLayers() { return mDataLayers; }

        const wxString GetVersion() { return version_string; };

        int GetSequenceDurationMS() const { return int(seq_duration * 1000); }
        double GetSequenceDurationDouble() const { return seq_duration; }
        wxString GetSequenceDurationString() const;

        void SetSequenceDurationMS(int length);
        void SetSequenceDuration(const wxString& length);
        void SetSequenceDuration(double length);

        const wxString GetSequenceTiming() const { return seq_timing; }
        void SetSequenceTiming(  const wxString& timing );
        int GetFrequency();

        const wxString GetSequenceType() const { return seq_type; }
        void SetSequenceType( const wxString& type );

        //const wxString GetMediaFile() const { return media_file; }
		AudioManager* GetMedia() const { return audio; }
		const wxString GetMediaFile() const { return media_file; }
		void SetMediaFile( const wxString& filename, bool overwrite_tags );

        wxString GetHeaderInfo(HEADER_INFO_TYPES node_type) { return header_info[node_type]; }
        void SetHeaderInfo(HEADER_INFO_TYPES node_type, const wxString& node_value);

        wxString GetImageDir(wxWindow* parent);
        void SetImageDir(const wxString& dir);

        void SetSequenceLoaded(bool value) { sequence_loaded = value; }
        bool GetSequenceLoaded() { return sequence_loaded; }

        void AddNewTimingSection(const std::string & interval_name, xLightsFrame* xLightsParent);
        void AddNewTimingSection(const std::string & interval_name, xLightsFrame* xLightsParent, std::vector<int> &starts,
                                 std::vector<int> &ends, std::vector<std::string> &labels);
        void AddFixedTimingSection(const std::string &interval_name, xLightsFrame* xLightsParent);
        void DeleteTimingSection(const std::string & section);
        void SetTimingSectionName(const std::string & section, const std::string & name);
        bool TimingAlreadyExists(const std::string & section, xLightsFrame* xLightsParent);
        wxArrayString GetTimingList() { return timing_list; }
        wxArrayString GetTimingList(SequenceElements& seq_elements);
        void ProcessAudacityTimingFiles(const wxString& dir, const wxArrayString& filenames, xLightsFrame* xLightsParent);
        void ProcessLorTiming(const wxString& dir, const wxArrayString& filenames, xLightsFrame* xLightsParent);
        void UpdateVersion();
        void UpdateVersion(const std::string &version);
        void AdjustEffectSettingsForVersion(SequenceElements& elements, xLightsFrame* xLightsParent);

        bool IsOpen() { return is_open; }
        bool HasAudioMedia() { return has_audio_media; }
        int GetNumModels() { return models.GetCount(); }
        bool WasConverted() { return was_converted; }
        void AcknowledgeConversion() { was_converted = false; }  // called to turn off conversion warning
        bool IsV3Sequence();
        bool NeedsTimesCorrected();
        void ConvertToFixedPointTiming();
        void SetMetaMP3Tags();

        void SetRenderMode(const wxString& mode);
        wxString GetRenderMode();

        int GetLastView();

        // static methods
        static void FixVersionDifferences(const wxString& filename);
        static void FixEffectPresets(wxXmlNode* effects_node);
        static bool IsXmlSequence(wxFileName &fname);

    protected:
    private:
        wxXmlDocument seqDocument;
        wxArrayString models;
        wxArrayString header_info;
        wxArrayString timing_list;
        wxString version_string;
        double seq_duration;
        wxString media_file;
        wxString seq_type;
        wxString seq_timing;
        wxString image_dir;
        bool is_open;
        bool has_audio_media;
        bool was_converted;
        bool sequence_loaded;  // flag to indicate the sequencer has been loaded with this xml data
        DataLayerSet mDataLayers;
		AudioManager* audio;

        void CreateNew();
        bool LoadSequence();
        bool LoadV3Sequence();
        bool Save();
        bool SaveCopy();
        void AddTimingDisplayElement( const wxString& name, const wxString& visible, const wxString& active );
        void AddDisplayElement( const wxString& name, const wxString& type, const wxString& visible, const wxString& collapsed, const wxString& active );
        wxXmlNode* AddElement( const wxString& name, const wxString& type );
        int AddColorPalette(StringIntMap &paletteCache, const wxString &palette);
        void AddEffect( wxXmlNode* node,
                       StringIntMap &paletteCache,
                        const wxString& name,
                        const wxString& data,
                        const wxString& protection,
                        const wxString& selected,
                        const wxString& id,
                        const wxString& start_time,
                        const wxString& end_time);
        void AddTimingEffect( wxXmlNode* node,
                              const wxString& label,
                              const wxString& protection,
                              const wxString& selected,
                              const wxString& start_time,
                              const wxString& end_time);
        wxXmlNode* AddChildXmlNode(wxXmlNode* node, const wxString& node_name, const wxString& node_data);
        wxXmlNode* AddChildXmlNode(wxXmlNode* node, const wxString& node_name);
        wxXmlNode* InsertChildXmlNode(wxXmlNode* node, wxXmlNode* following_node, const wxString& node_name);
        wxXmlNode* AddFixedTiming( const wxString& name, const wxString& timing );
        void SetNodeContent(wxXmlNode* node, const wxString& content);
        void CleanUpEffects();
        void UpdateNextId( const wxString& value );

        void FixVersionDifferences();

        void SetSequenceDuration(const wxString& length, wxXmlNode* node);

        static wxString InsertMissing(wxString str, wxString missing_array, bool INSERT);

        void WriteEffects(EffectLayer *layer,
                          wxXmlNode *effect_layer_node,
                          StringIntMap &colorPalettes,
                          wxXmlNode* colorPalette_node,
                          StringIntMap &effectStrings,
                          wxXmlNode* effectDB_Node);
};

#endif // XLIGHTSXMLFILE_H
