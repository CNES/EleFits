// Copyright (C) 2019-2022, CNES and contributors (for the Euclid Science Ground Segment)
// This file is part of EleFits <github.com/CNES/EleFits>
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef _ELEFITS_BINTABLECOLUMNS_H
#define _ELEFITS_BINTABLECOLUMNS_H

#include "EleFits/ColumnKey.h"
#include "EleFits/FileMemSegments.h"
#include "EleFitsData/Column.h"
#include "EleFitsData/DataUtils.h" // TypedKey

#include <fitsio.h>
#include <functional>

namespace Euclid {
namespace Fits {

/**
 * @ingroup bintable_handlers
 * @brief Column-wise reader-writer for the binary table data unit.
 * @details
 * For flexibility, this handler class provides many methods to read and write data,
 * but they are just different flavors of the following few services:
 * - Read/write a single column completely;
 * - Read/write a segment (i.e. consecutive rows) of a single column;
 * - Read/write a sequence of columns completely;
 * - Read/write a sequence of column segments (same row interval for all the columns).
 * 
 * For reading, new columns can be either returned, or existing columns can be filled.
 * Columns can be specified either by their name or index;
 * using index is faster because names are internally converted to indices anyway, via a read operation.
 * When filling an existing column, the name of the column can also be used to specify the column to be read.
 * 
 * When writing, if more rows are needed, they are automatically filled with zeros.
 * 
 * In the FITS file, binary tables are written row-wise, i.e. values of a row are contiguous in the file.
 * As of today, in memory, values are stored column-wise (in `Column`) for convenience,
 * to avoid heterogeneous containers as much as possible.
 * This implies that read and write functions jump from one disk or memory adress to another all the time,
 * which costs a lot of resources.
 * To save on I/Os, an internal buffer is instantiated by CFITSIO.
 * As opposed to methods to read and write a single column,
 * methods to read and write several columns take advantage of the internal buffer.
 * It is therefore much more efficient to use those than to chain several calls to methods for single columns.
 * Depending on the table width, the speed-up can reach several orders of magnitude.
 * 
 * Method to read and write columns conform to the following naming convention:
 * - Starts with `read` or `write`;
 * - Contains `segment` for reading or writing segments;
 * - Contains `n` for reading or writing several columns;
 * - Contains `to` for filling an existing column.
 * 
 * For example, `read_n_segments_to()` is a method to read a sequence of segments into existing `Column` objects.
 * 
 * For working with segments, row indices are specified as `Segment`s or `FileMemSegments`.
 * 
 * For example, assume we want to concatenate rows 11 to 50 of a 3-column binary table into some `std::vector`.
 * Here is an option:
 * \code
 * // Specs
 * const Segment rows {11, 50};
 * const long column_count = 3;
 * const long row_count = rows.size();
 * 
 * // Data container for all columns
 * std::vector<float> data(row_count * column_count);
 * 
 * // Contiguous views
 * PtrColumn<float> one({"ONE", "", 1}, row_count, &data[0]);
 * PtrColumn<float> two({"TWO", "", 1}, row_count, &data[row_count]);
 * PtrColumn<float> three({"THREE", "", 1}, row_count, &data[row_count * 2]);
 * 
 * // In-place reading
 * columns.read_n_segments_to(rows, one, two, three);
 * \endcode
 */
class BintableColumns {
private:

  friend class BintableHdu;

  /**
   * @brief Constructor.
   */
  BintableColumns(fitsfile*& fptr, std::function<void(void)> touch, std::function<void(void)> edit);

public:

  /// @group_properties

  /**
   * @brief Get the current number of columns.
   */
  long read_column_count() const;

  /**
   * @brief Get the current number of rows.
   */
  long read_row_count() const;

  /**
   * @brief Get the number of rows in the internal buffer.
   * @details
   * CFITSIO internally implements a buffer to read and write data units efficiently.
   * To optimize its usage, columns should be read and written by chunks of the buffer size at most.
   */
  long read_buffer_row_count() const;

  /**
   * @brief Check whether the HDU contains a given column.
   * @warning This is a read operation.
   */
  bool has(const std::string& name) const;

  /**
   * @brief Get the index of the column with given name.
   */
  long read_index(const std::string& name) const;

  /**
   * @brief Get the indices of the columns with given names.
   */
  std::vector<long> read_n_indices(const std::vector<std::string>& names) const;

  /**
   * @brief Get the name of the column with given index.
   */
  std::string read_name(long index) const;

  /**
   * @brief Get the names of all the columns.
   */
  std::vector<std::string> read_all_names() const;

  /**
   * @brief Rename the column with given name or index.
   * @param key The name or 0-based index of the column to be read
   * @param name The name to be given
   * @warning This is a write operation.
   */
  void update_name(ColumnKey key, const std::string& name) const
  {
    return rename(key, name);
  }

  /// @}
  /**
   * @name Read a single column
   */
  /// @{

  /**
   * @brief Read the info of a column.
   * @param key The name or 0-based index of the column to be read
   */
  template <typename T, long N = 1>
  ColumnInfo<T, N> read_info(ColumnKey key) const;

  /**
   * @brief Read the column with given name or index.
   * @param key The name or 0-based index of the column to be read
   * @details
   * There are several ways to read a column, which can be specified either by its name or 0-based index.
   * The simplest way is to read the whole column as a new `VecColumn` with methods `read()`.
   * In this case, the value type is given as the template parameter.
   * In order to store the column data in an existing `Column` (e.g. `PtrColumn`), similar methods `read_to()` should be used.
   * In this case, the value type is deduced and should not be specified.
   * 
   * Example usages:
   * \code
   * // Create a new Column
   * auto from_name = columns.read<float>("RA");
   * auto from_index = columns.read<float>(1);
   * 
   * // Concatenate two columns into an existing Column
   * long row_count = read_row_count();
   * std::vector<float> values(row_count * 2);
   * PtrColumn<float> ra({"RA", "deg", 1}, row_count, &values[0]);
   * PtrColumn<float> dec({"DEC", "deg", 1}, row_count, &values[row_count]);
   * columns.read_to("RA", ra);
   * columns.read_to("DEC", dec);
   * \endcode
   * 
   * @warning
   * Methods `read_to()` do not allocate memory: the user must ensure that enough space has been allocated previously.
   */
  template <typename T, long N = 1>
  VecColumn<T, N> read(ColumnKey key) const;

  /**
   * @brief Read a column into an existing `Column`.
   * @param column The `Column` object to which data should be written,
   * name of which is used to specify the column to be read
   * @copydetails read()
   */
  template <typename TColumn>
  void read_to(TColumn& column) const;

  /**
   * @brief Read the column with given name or index into an existing `Column`.
   * @param key The name or 0-based index of the column to be read
   * @param column The `Column` object to which data should be written
   * (`column.info().name` is not used by the method and can be different from the `name` parameter)
   * @copydetails read()
   */
  template <typename TColumn>
  void read_to(ColumnKey key, TColumn& column) const;

  /// @}
  /**
   * @name Read a single column segment
   */
  /// @{

  /**
   * @brief Read the segment of a column specified by its name or index.
   * @param rows The included lower and upper bounds of the row indices to be read
   * @param key The name or 0-based index of the column to be read
   * @param column The preexisting `Column` to be filled
   * @details
   * Methods to read column segments are similar to methods to read complete columns (see `read()`).
   * They accept an additional parameter to specify the rows to be read, as the bounds of a closed interval.
   * 
   * Example usages:
   * \code
   * // Create a new Column
   * auto segment = columns.read_segment<float>({10, 50}, "NAME");
   * 
   * // Read into an existing Column
   * // This is a more complex example which demonstrates the use of offsets
   * Segment source_bounds = {10, 50};
   * long destination_row = 20;
   * std::vector<float> values(100);
   * PtrColumn<float> segment({"NAME", "m/s", 1}, 20, &values[destination_row]);
   * columns.read_segment_to(source_bounds, "NAME", segment);
   * \endcode
   * @see read()
   */
  template <typename T, long N = 1>
  VecColumn<T, N> read_segment(const Segment& rows, ColumnKey key) const;

  /**
   * @brief Read the segment of a column into an existing `Column`.
   * @copydetails read_segment()
   */
  template <typename TColumn>
  void read_segment_to(FileMemSegments rows, TColumn& column) const;

  /**
   * @brief Read the segment of a column specified by its name or index into an existing `Column`.
   * @copydetails read_segment()
   */
  template <typename TColumn>
  void read_segment_to(FileMemSegments rows, ColumnKey key, TColumn& column) const;

  /// @}
  /**
   * @name Read a sequence of columns
   */
  /// @{

  /**
   * @brief Read a tuple of columns with given names or indices.
   * @details
   * Example usages:
   * \code
   * // Heterogeneous sequence (returns a tuple)
   * auto columns = ext.read_n(as<int>("A"), as<float, 2>("B"), as<std::string>("C"));
   * auto columns = ext.read_n(as<int>(0), as<float, 2>(3), as<std::string>(4));
   * 
   * // Homogeneous sequence (returns a vector)
   * auto columns = ext.read_n<int, 2>({"A", "B", "C"});
   * auto columns = ext.read_n<int, 2>({0, 3, 4});
   * \endcode
   * @warning
   * Multidimensional columns are read as vector columns as of today.
   */
  template <typename TKey, typename... Ts> // FIXME add long... Ns
  std::tuple<VecColumn<Ts, 1>...> read_n(const TypedKey<Ts, TKey>&... keys) const; // FIXME Ns in TypedKey and as()

  /**
   * @brief Read a vector of columns with given names or indices.
   * @copydetails read_n()
   */
  template <typename T, long N = 1>
  std::vector<VecColumn<T, N>> read_n(std::vector<ColumnKey> keys) const; // TODO return a ColumnVec?

  /**
   * @brief Read a sequence of columns into existing `Column`s.
   * @copydetails read_n()
   */
  template <typename TSeq>
  void read_n_to(TSeq&& columns) const;

  /**
   * @brief Read a sequence of columns into existing `Column`s.
   * @copydetails read_n()
   */
  template <typename... TColumns>
  void read_n_to(TColumns&... columns) const;

  /**
   * @brief Read a sequence of columns with given names or indices into existing `Column`s.
   * @copydetails read_n()
   */
  template <typename TSeq>
  void read_n_to(std::vector<ColumnKey> keys, TSeq&& columns) const;

  /**
   * @brief Read a sequence of columns with given names or indices into existing `Column`s.
   * @copydetails read_n()
   */
  template <typename... TColumns>
  void read_n_to(std::vector<ColumnKey> keys, TColumns&... columns) const;

  /// @}
  /**
   * @name Read a sequence of column segments
   */
  /// @{

  /**
   * @brief Read segments of columns specified by their names or indices.
   * @details
   * The rows to be read in the table are specified as a `Segment` object, that is, a lower and upper bounds.
   * The same bounds are used for all columns.
   * @warning
   * Multidimensional columns are read as vector columns as of today.
   */
  template <typename TKey, typename... Ts> // FIXME Ns
  std::tuple<VecColumn<Ts, 1>...> read_n_segments(Segment rows, const TypedKey<Ts, TKey>&... keys) const;

  /**
   * @copydoc read_n_segments
   */
  template <typename T, long N = 1>
  std::vector<VecColumn<T, N>> read_n_segments(Segment rows, std::vector<ColumnKey> keys) const;

  /**
   * @brief Read segments of columns into existing `Column`s.
   * @copydetails read_n_segments()
   */
  template <typename TSeq>
  void read_n_segments_to(FileMemSegments rows, TSeq&& columns) const;

  /**
   * @brief Read segments of columns into existing `Column`s.
   * @copydetails read_n_segments()
   */
  template <typename... TColumns>
  void read_n_segments_to(FileMemSegments rows, TColumns&... columns) const;

  /**
   * @brief Read segments of columns specified by their names or indices into existing `Column`s.
   * @copydetails read_n_segments()
   */
  template <typename TSeq>
  void read_n_segments_to(FileMemSegments rows, std::vector<ColumnKey> keys, TSeq&& columns) const;

  /**
   * @brief Read segments of columns specified by their names or indices into existing `Column`s.
   * @copydetails read_n_segments()
   */
  template <typename... TColumns>
  void read_n_segments_to(FileMemSegments rows, std::vector<ColumnKey> keys, TColumns&... columns) const;

  /// @}
  /**
   * @name Write a single column
   */
  /// @{

  /**
   * @brief Write a column.
   */
  template <typename TColumn>
  void write(const TColumn& column) const;

  /**
   * @brief Append a null-initialized column.
  */
  template <typename TInfo>
  void append_null(const TInfo& info) const
  {
    return insert_null(-1, info);
  }

  /**
   * @brief Append and fill a column.
   */
  template <typename TColumn>
  void append(const TColumn& column) const
  {
    return insert(-1, column);
  }

  /**
   * @brief Insert a null-initialized column.
   * @param index The 0-based column index, which may be &ge; 0 or -1 to append the column at the end
   * @param info The column info
   */
  template <typename TInfo>
  void insert_null(long index, const TInfo& info) const;

  /**
   * @brief Insert and fill a column.
   */
  template <typename TColumn>
  void insert(long index, const TColumn& column) const
  {
    insert_null(index, column.info());
    write(column);
  }

  /**
   * @brief Remove a column specified by its name or index.
   */
  void remove(ColumnKey key) const;

  /// @}
  /**
   * @name Write a single column segment
   */
  /// @{

  /**
   * @brief Write a column segment.
   * @param rows The row index range to be written
   * @param column The column to be written
   */
  template <typename TColumn>
  void write_segment(FileMemSegments rows, const TColumn& column) const;

  /// @}
  /**
   * @name Write a sequence of columns
   */
  /// @{

  /**
   * @brief Write several columns.
   * 
   * The column names are used to deduce their indices.
   * If the name is not found, an error is thrown.
   */
  template <typename TSeq>
  void write_n(TSeq&& columns) const;

  /**
   * @copydoc write_n()
   */
  template <typename... TColumns>
  void write_n(const TColumns&... columns) const;

  /**
   * @brief Append a sequence of null-initialized columns.
   * @param infos The column infos
   */
  template <typename... TInfos>
  void append_n_null(TInfos&&... infos) const
  {
    return insert_n_null(-1, std::forward<TInfos>(infos)...);
  }

  /**
   * @brief Insert a sequence of null-initialized columns.
   * @param index The 0-based index of the first column to be added, which may be &ge; 0 or -1 to append the columns at the end
   * @param infos The column infos
   */
  template <typename TSeq>
  void insert_n_null(long index, TSeq&& infos) const;

  /**
   * @copydoc insert_n_null
   */
  template <typename... TInfos>
  void insert_n_null(long index, const TInfos&... infos) const;

  /**
   * @brief Remove a sequence of columns specified by their names or indices.
   */
  void remove_n(std::vector<ColumnKey> keys) const;

  /// @}
  /**
   * @name Write a sequence of column segments
   */
  /// @{

  /**
   * @brief Write a sequence of segments.
   * @param rows The mapping between the in-file and in-memory rows
   * @param columns The columns to be written
   * Segments can be written in already initialized columns with `write_n_segments()`
   * or in new columns with `append_segment_seq()`.
   */
  template <typename TSeq>
  void write_n_segments(FileMemSegments rows, TSeq&& columns) const;

  /**
   * @copydoc write_n_segments
   */
  template <typename... TColumns>
  void write_n_segments(FileMemSegments rows, const TColumns&... columns) const;

  /// @group_deprecated

  /**
   * @deprecated
   */
  [[deprecated]] long readColumnCount() const
  {
    return read_column_count();
  }

  /**
   * @deprecated
   */
  [[deprecated]] long readRowCount() const
  {
    return read_row_count();
  }

  /**
   * @deprecated
   */
  [[deprecated]] long readBufferRowCount() const
  {
    return read_buffer_row_count();
  }

  /**
   * @deprecated
   */
  [[deprecated]] long readIndex(const std::string& name) const
  {
    return read_index(name);
  }

  /**
   * @deprecated
   */
  [[deprecated("Use read_n_indices()")]] std::vector<long> readIndices(const std::vector<std::string>& names) const
  {
    return read_n_indices(names);
  }

  /**
   * @deprecated
   */
  [[deprecated]] std::string readName(long index) const
  {
    return read_name(index);
  }

  /**
   * @deprecated
   */
  [[deprecated]] std::vector<std::string> readAllNames() const
  {
    return read_all_names();
  }

  /**
   * @deprecated
   */
  template <typename T, long N = 1>
  [[deprecated]] ColumnInfo<T, N> readInfo(ColumnKey key) const
  {
    return read_info<T, N>(key);
  }

  /**
   * @deprecated
   */
  [[deprecated("Use update_name()")]] void rename(ColumnKey key, const std::string& name) const; // FIXME update name?

  /**
   * @deprecated
   */
  template <typename... TArgs>
  [[deprecated]] void readTo(TArgs&&... args)
  {
    return read_to(std::forward<TArgs>(args)...); // FIXME test
  }

  /**
   * @deprecated
   */
  template <typename T, long N = 1>
  [[deprecated]] VecColumn<T, N> readSegment(const Segment& rows, ColumnKey key) const
  {
    return read_segment<T, N>(rows, key);
  }

  template <typename... TArgs>
  [[deprecated]] void readSegmentTo(TArgs&&... args) const
  {
    return read_segment_to(std::forward<TArgs>(args)...);
  }

  /**
   * @deprecated
   */
  template <typename TKey, typename... Ts> // FIXME add long... Ns
  [[deprecated]] std::tuple<VecColumn<Ts, 1>...> readSeq(const TypedKey<Ts, TKey>&... keys) const
  {
    return read_n(keys...);
  }

  /**
   * @deprecated
   */
  template <typename T, long N = 1>
  [[deprecated]] std::vector<VecColumn<T, N>> readSeq(std::vector<ColumnKey> keys) const
  {
    return read_n<T, N>(keys);
  }

  /**
   * @deprecated
   */
  template <typename... TArgs>
  [[deprecated]] void readSeqTo(TArgs&... args) const
  {
    return read_n_to(std::forward<TArgs>(args)...);
  }

  /**
   * @deprecated
   */
  template <typename TKey, typename... Ts>
  [[deprecated]] std::tuple<VecColumn<Ts, 1>...> readSegmentSeq(Segment rows, const TypedKey<Ts, TKey>&... keys) const
  {
    return read_n_segments(rows, keys...);
  }

  /**
   * @deprecated
   */
  template <typename T, long N = 1>
  [[deprecated]] std::vector<VecColumn<T, N>> readSegmentSeq(Segment rows, const std::vector<ColumnKey> keys) const
  {
    return read_n_segments<T, N>(rows, keys);
  }

  /**
   * @deprecated
   */
  template <typename... TArgs>
  [[deprecated]] void readSegmentSeqTo(TArgs&&... args) const
  {
    return read_n_segments_to(std::forward<TArgs>(args)...);
  }

  /**
   * @deprecated Replaced with append_null() and insert_null()
  */
  template <typename TInfo>
  [[deprecated("Use append_null() or insert_null()")]] void init(const TInfo& info, long index = -1) const;

  /**
   * @deprecated
   */
  template <typename TColumn>
  [[deprecated]] void writeSegment(FileMemSegments rows, const TColumn& column) const
  {
    return write_segment(rows, column);
  }

  /**
   * @deprecated
   */
  template <typename... TArgs>
  [[deprecated]] void writeSeq(const TArgs&... args) const
  {
    return write_n(std::forward<TArgs>(args)...);
  }

  /**
   * @deprecated Use insert_n_null()
   */
  template <typename... TArgs>
  [[deprecated("Use insert_n_null()")]] void initSeq(TArgs&&... args) const
  {
    return insert_n_null(std::forward<TArgs>(args)...);
  }

  /**
   * @deprecated
   */
  [[deprecated]] void removeSeq(std::vector<ColumnKey> keys) const
  {
    return remove_n(keys);
  }

  /**
   * @deprecated
   */
  template <typename... TArgs>
  [[deprecated]] void writeSegmentSeq(TArgs&&... args) const
  {
    return write_n_segments(std::forward<TArgs>(args)...);
  }

  /// @}

private:

  /**
   * @brief The fitsfile.
   */
  fitsfile*& m_fptr;

  /**
   * @brief The function to declare that the header was touched.
   */
  std::function<void(void)> m_touch;

  /**
   * @brief The function to declare that the header was edited.
   */
  std::function<void(void)> m_edit;
};

/**
 * @relates Column
 * @brief Get the common number of rows of a sequence of columns.
 * 
 * Throws if columns do not have the same size.
 */
template <typename TSeq>
long columns_row_count(TSeq&& columns);

/**
 * @deprecated
 */
template <typename TSeq>
long columnsRowCount(TSeq&& columns)
{
  return columns_row_count(std::forward<TSeq>(columns));
}

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_BINTABLECOLUMNS_IMPL
#include "EleFits/impl/BintableColumns.hpp"
#undef _ELEFITS_BINTABLECOLUMNS_IMPL
/// @endcond

#endif
