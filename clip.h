// Clip Library
// Copyright (c) 2015-2016 David Capello

#ifndef CLIP_H_INCLUDED
#define CLIP_H_INCLUDED
#pragma once

#include <cassert>
#include <string>

namespace clip {

  // ======================================================================
  // Low-level API to lock the clipboard/pasteboard and modify it
  // ======================================================================

  // Clipboard format identifier.
  typedef size_t format;

  class image;

  class lock {
  public:
    lock(void* native_handle);
    ~lock();

    // Returns true if we've locked the clipboard successfully in
    // lock() constructor.
    bool locked() const;

    // Clears the clipboard content. If you don't clear the content,
    // previous clipboard content (in unknown formats) could persist
    // after the unlock.
    bool clear();

    // Returns true if the clipboard can be converted to the given
    // format.
    bool is_convertible(format f) const;
    bool set_data(format f, const char* buf, size_t len);
    bool get_data(format f, char* buf, size_t len) const;
    size_t get_data_length(format f) const;

    // For images
    bool set_image(const image& image);
    bool get_image(image& image) const;

  private:
    class impl;
    impl* p;
  };

  format register_format(const std::string& name);

  // This format is when the clipboard has no content.
  format empty_format();

  // When the clipboard has UTF8 text.
  format text_format();

  // When the clipboard has an image.
  format image_format();

  // Returns true if the clipboard has content of the given type.
  bool has(format f);

  // Clears the clipboard content.
  bool clear();

  // ======================================================================
  // Error handling
  // ======================================================================

  enum ErrorCode {
    CannotLock,
    PixelFormatNotSupported,
  };

  typedef void (*error_handler)(ErrorCode code);

  void set_error_handler(error_handler f);
  error_handler get_error_handler();

  // ======================================================================
  // Text
  // ======================================================================

  // High-level API to put/get UTF8 text in/from the clipboard. These
  // functions returns false in case of error.
  bool set_text(const std::string& value);
  bool get_text(std::string& value);

  // ======================================================================
  // Image
  // ======================================================================

  struct image_spec {
    unsigned long width;
    unsigned long height;
    unsigned long bits_per_pixel;
    unsigned long bytes_per_row;
    unsigned long red_mask;
    unsigned long green_mask;
    unsigned long blue_mask;
    unsigned long alpha_mask;
    unsigned long red_shift;
    unsigned long green_shift;
    unsigned long blue_shift;
    unsigned long alpha_shift;
  };

  class image {
  public:
    // Constructor to use get_image()
    image() : m_data(nullptr), m_own_data(false) {
    }

    // Constructor to use set_image()
    image(char* data, bool own_data, const image_spec& spec)
      : m_data(data), m_own_data(own_data), m_spec(spec) {
    }

    ~image() {
      if (m_own_data) {
        delete[] m_data;
        m_data = nullptr;
      }
    }

    char* data() const { return m_data; }
    const image_spec& spec() const { return m_spec; }

  private:
    char* m_data;
    bool m_own_data;
    image_spec m_spec;
  };

  // High-level API to set/get an image in/from the clipboard. These
  // functions returns false in case of error.
  bool set_image(const image& img);
  bool get_image(image& img);

} // namespace clip

#endif // CLIP_H_INCLUDED
