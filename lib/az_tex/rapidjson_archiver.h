#pragma once

#include <cstddef>
#include <string>
#include <functional>

namespace az::tex::impl {
    /// Represents a JSON reader which implements Archiver concept.
    class JsonReader {
    public:
        /// Constructor.
        /**
            \param json A non-const source json string for in-situ parsing.
            \note in-situ means the source JSON string will be modified after parsing.
        */
        JsonReader(const char *json);

        /// Destructor.
        ~JsonReader();

        // Archive concept
        operator bool() const { return !mError; }

        JsonReader &StartObject();

        JsonReader &TraverseAllMembers(const std::function<void(const std::string &key)> &);

        JsonReader &Member(const char *name);

        bool HasMember(const char *name) const;

        JsonReader &EndObject();

        JsonReader &StartArray(size_t *size = 0);

        JsonReader &EndArray();

        JsonReader &operator&(bool &b);

        JsonReader &operator&(unsigned &u);

        JsonReader &operator&(int &i);

        JsonReader &operator&(double &d);

        JsonReader &operator&(std::string &s);

        JsonReader &SetNull();

        bool IsReader = true;
    private:
        JsonReader(const JsonReader &);

        JsonReader &operator=(const JsonReader &);

        void Next();

        // PIMPL
        void *mDocument;              ///< DOM result of parsing.
        void *mStack;                 ///< Stack for iterating the DOM
        bool mError;                  ///< Whether an error has occurred.
    };

    class JsonWriter {
    public:
        /// Constructor.
        JsonWriter();

        /// Destructor.
        ~JsonWriter();

        /// Obtains the serialized JSON string.
        const char *GetString() const;

        // Archive concept
        operator bool() const { return true; }

        JsonWriter &StartObject();

        JsonWriter &Member(const char *name);

        bool HasMember(const char *name) const;

        JsonWriter &EndObject();

        JsonWriter &StartArray(size_t *size = 0);

        JsonWriter &EndArray();

        JsonWriter &operator&(const bool &b);

        JsonWriter &operator&(const unsigned &u);

        JsonWriter &operator&(const int &i);

        JsonWriter &operator&(const double &d);

        JsonWriter &operator&(const std::string &s);

        JsonWriter &SetNull();

        bool IsReader = false;
    private:
        JsonWriter(const JsonWriter &);

        JsonWriter &operator=(const JsonWriter &);

        // PIMPL idiom
        void *mWriter;      ///< JSON writer.
        void *mStream;      ///< Stream buffer.
    };
}
