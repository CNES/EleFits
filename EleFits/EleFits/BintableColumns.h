/**
 * @copyright (C) 2012-2020 Euclid Science Ground Segment
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 */

#ifndef _ELEFITS_BINTABLECOLUMNS_H
#define _ELEFITS_BINTABLECOLUMNS_H

#include "EleFits/ColumnKey.h"
#include "EleFits/FileMemSegments.h"
#include "EleFitsData/Column.h"

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
 * In the Fits file, binary tables are written row-wise, i.e. values of a row are contiguous in the file.
 * As of today, in memory, values are stored column-wise (in `Column`) for convenience,
 * to avoid heterogeneous containers as much as possible.
 * This implies that read and write functions jump from one disk or memory adress to another all the time,
 * which costs a lot of resources.
 * To save on I/Os, an internal buffer is instantiated by CFitsIO.
 * As opposed to methods to read and write a single column,
 * methods to read and write several columns take advantage of the internal buffer.
 * It is therefore much more efficient to use those than to chain several calls to methods for single columns.
 * Depending on the table width, the speed-up can reach several orders of magnitude.
 * 
 * Method to read and write columns conform to the following naming convention:
 * - Start with `read` or `write`;
 * - Contain `Segment` for reading or writing segments;
 * - Contain `Seq` for reading or writing several columns;
 * - Contain `To` for filling an existing column.
 * 
 * For example, `readSegmentSeqTo()` is a method to read a sequence of segments into existing `Column` objects.
 * 
 * For working with segments, row indices are specified as `Segment`s or `FileMemSegments`.
 * 
 * For example, assume we want to concatenate rows 11 to 50 of a 3-column binary table into some `std::vector`.
 * Here is an option:
 * \code
 * // Specs
 * const Segment rows {11, 50};
 * const long columnCount = 3;
 * const long rowCount = rows.size();
 * 
 * // Data container for all columns
 * std::vector<float> data(rowCount * columnCount);
 * 
 * // Contiguous views
 * PtrColumn<float> one({"ONE", "", 1}, rowCount, &data[0]);
 * PtrColumn<float> two({"TWO", "", 1}, rowCount, &data[rowCount]);
 * PtrColumn<float> three({"THREE", "", 1}, rowCount, &data[rowCount * 2]);
 * 
 * // In-place reading
 * columnCount.readSegmentSeqTo(rows, one, two, three);
 * \endcode
 */
class BintableColumns {

private:
  friend class BintableHdu;

  /**
   * @brief Constructor.
   */
  BintableColumns(fitsfile*& fptr, std::function<void(void)> touchFunc, std::function<void(void)> editFunc);

public:
  /**
   * @name Column metadata.
   */
  /// @{

  /**
   * @brief Get the current number of columns.
   */
  long readColumnCount() const;

  /**
   * @brief Get the current number of rows.
   */
  long readRowCount() const;

  /**
   * @brief Get the number of rows in the internal buffer.
   * @details
   * CFitsIO internally implements a buffer to read and write data units efficiently.
   * To optimize its usage, columns should be read and written by chunks of the buffer size at most.
   */
  long readBufferRowCount() const;

  /**
   * @brief Check whether the HDU contains a given column.
   * @warning This is a read operation.
   */
  bool has(const std::string& name) const;

  /**
   * @brief Get the index of the column with given name.
   */
  long readIndex(const std::string& name) const;

  /**
   * @brief Get the indices of the columns with given names.
   */
  std::vector<long> readIndices(const std::vector<std::string>& names) const;

  /**
   * @brief Get the name of the column with given index.
   */
  std::string readName(long index) const;

  /**
   * @brief Get the names of all the columns.
   */
  std::vector<std::string> readAllNames() const;

  /**
   * @brief Rename the column with given name or index.
   * @param key The name or 0-based index of the column to be read
   * @param newName The name to be given
   * @warning This is a write operation.
   */
  void rename(ColumnKey key, const std::string& newName) const;

  /// @}
  /**
   * @name Read a single column.
   */
  /// @{

  /**
   * @brief Read the info of a column.
   * @param key The name or 0-based index of the column to be read
   */
  template <typename T>
  ColumnInfo<T> readInfo(ColumnKey key) const;

  /**
   * @brief Read the column with given name or index.
   * @param key The name or 0-based index of the column to be read
   * @details
   * There are several ways to read a column, which can be specified either by its name or 0-based index.
   * The simplest way is to read the whole column as a new `VecColumn` with methods `read()`.
   * In this case, the value type is given as the template parameter.
   * In order to store the column data in an existing `Column` (e.g. `PtrColumn`), similar methods `readTo()` should be used.
   * In this case, the value type is deduced and should not be specified.
   * 
   * Example usages:
   * \code
   * // Create a new Column
   * auto fromName = columns.read<float>("RA");
   * auto fromIndex = columns.read<float>(1);
   * 
   * // Concatenate two columns into an existing Column
   * long rowCount = readRowCount();
   * std::vector<float> values(rowCount * 2);
   * PtrColumn<float> ra({"RA", "deg", 1}, rowCount, &values[0]);
   * PtrColumn<float> dec({"DEC", "deg", 1}, rowCount, &values[rowCount]);
   * columns.readTo("RA", ra);
   * columns.readTo("DEC", dec);
   * \endcode
   * 
   * @warning
   * Methods `readTo()` do not allocate memory: the user must ensure that enough space has been allocated previously.
   */
  template <typename T>
  VecColumn<T> read(ColumnKey key) const;

  /**
   * @brief Read a column into an existing `Column`.
   * @param column The `Column` object to which data should be written,
   * name of which is used to specify the column to be read
   * @copydetails read()
   */
  template <typename T>
  void readTo(Column<T>& column) const;

  /**
   * @brief Read the column with given name or index into an existing `Column`.
   * @param key The name or 0-based index of the column to be read
   * @param column The `Column` object to which data should be written
   * (`column.info().name` is not used by the method and can be different from the `name` parameter)
   * @copydetails read()
   */
  template <typename T>
  void readTo(ColumnKey key, Column<T>& column) const;

  /// @}
  /**
   * @name Read a single column segment.
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
   * auto segment = columns.readSegment<float>({10, 50}, "NAME");
   * 
   * // Read into an existing Column
   * // This is a more complex example which demonstrates the use of offsets
   * Segment sourceBounds = {10, 50};
   * long destinationRow = 20;
   * std::vector<float> values(100);
   * PtrColumn<float> segment({"NAME", "m/s", 1}, 20, &values[destinationRow]);
   * columns.readSegmentTo(sourceBounds, "NAME", segment);
   * \endcode
   * @see read()
   */
  template <typename T>
  VecColumn<T> readSegment(const Segment& rows, ColumnKey key) const;

  /**
   * @brief Read the segment of a column into an existing `Column`.
   * @copydetails readSegment()
   */
  template <typename T>
  void readSegmentTo(FileMemSegments rows, Column<T>& column) const;

  /**
   * @brief Read the segment of a column specified by its name or index into an existing `Column`.
   * @copydetails readSegment()
   */
  template <typename T>
  void readSegmentTo(FileMemSegments rows, ColumnKey key, Column<T>& column) const;

  /// @}
  /**
   * @name Read a sequence of columns.
   */
  /// @{

  /**
   * @brief Read a tuple of columns with given names or indices.
   * @details
   * Example usages:
   * \code
   * // Heterogeneous sequence (returns a tuple)
   * auto columns = ext.readSeq(as<int>("A"), as<float>("B"), as<std::string>("C"));
   * auto columns = ext.readSeq(as<int>(0), as<float>(3), as<std::string>(4));
   * 
   * // Homogeneous sequence (returns a vector)
   * auto columns = ext.readSeq<int>({"A", "B", "C"});
   * auto columns = ext.readSeq<int>({0, 3, 4});
   * \endcode
   */
  template <typename TKey, typename... Ts>
  std::tuple<VecColumn<Ts>...> readSeq(const TypedKey<Ts, TKey>&... keys) const;

  /**
   * @brief Read a vector of columns with given names or indices.
   * @copydetails readSeq()
   */
  template <typename T>
  std::vector<VecColumn<T>> readSeq(std::vector<ColumnKey> keys) const; // TODO return a ColumnVec?

  /**
   * @brief Read a sequence of columns into existing `Column`s.
   * @copydetails readSeq()
   */
  template <typename TSeq>
  void readSeqTo(TSeq&& columns) const;

  /**
   * @brief Read a sequence of columns into existing `Column`s.
   * @copydetails readSeq()
   */
  template <typename... Ts>
  void readSeqTo(Column<Ts>&... columns) const;

  /**
   * @brief Read a sequence of columns with given names or indices into existing `Column`s.
   * @copydetails readSeq()
   */
  template <typename TSeq>
  void readSeqTo(std::vector<ColumnKey> keys, TSeq&& columns) const;

  /**
   * @brief Read a sequence of columns with given names or indices into existing `Column`s.
   * @copydetails readSeq()
   */
  template <typename... Ts>
  void readSeqTo(std::vector<ColumnKey> keys, Column<Ts>&... columns) const;

  /// @}
  /**
   * @name Read a sequence of column segments.
   */
  /// @{

  /**
   * @brief Read segments of columns specified by their names or indices.
   * @details
   * The rows to be read in the table are specified as a `Segment` object, that is, a lower and upper bounds.
   * The same bounds are used for all columns.
   */
  template <typename TKey, typename... Ts>
  std::tuple<VecColumn<Ts>...> readSegmentSeq(Segment rows, const TypedKey<Ts, TKey>&... keys) const;

  /**
   * @copydoc readSegmentSeq
   */
  template <typename T>
  std::vector<VecColumn<T>> readSegmentSeq(Segment rows, std::vector<ColumnKey> keys) const;

  /**
   * @brief Read segments of columns into existing `Column`s.
   * @copydetails readSegmentSeq()
   */
  template <typename TSeq>
  void readSegmentSeqTo(FileMemSegments rows, TSeq&& columns) const;

  /**
   * @brief Read segments of columns into existing `Column`s.
   * @copydetails readSegmentSeq()
   */
  template <typename... Ts>
  void readSegmentSeqTo(FileMemSegments rows, Column<Ts>&... columns) const;

  /**
   * @brief Read segments of columns specified by their names or indices into existing `Column`s.
   * @copydetails readSegmentSeq()
   */
  template <typename TSeq>
  void readSegmentSeqTo(FileMemSegments rows, std::vector<ColumnKey> keys, TSeq&& columns) const;

  /**
   * @brief Read segments of columns specified by their names or indices into existing `Column`s.
   * @copydetails readSegmentSeq()
   */
  template <typename... Ts>
  void readSegmentSeqTo(FileMemSegments rows, std::vector<ColumnKey> keys, Column<Ts>&... columns) const;

  /// @}
  /**
   * @name Write a single column.
   */
  /// @{

  /**
   * @brief Write a column.
   */
  template <typename T>
  void write(const Column<T>& column) const;

  /**
   * @brief Append or insert a column, which was not previously initialized.
   * @param info The column info
   * @param index The 0-based column index, which may be >= 0 or -1 to append the column at the end
   */
  template <typename T>
  void init(const ColumnInfo<T>& info, long index = -1) const;

  /**
   * @brief Remove a column specified by its name or index.
   */
  void remove(ColumnKey key) const;

  /// @}
  /**
   * @name Write a single column segment.
   */
  /// @{

  /**
   * @brief Write a column segment.
   * @param firstRow The 0-based index in the binary table of the first row to be written
   * @param column The segment to be written
   * @details
   * Analogously to `write()` and `insert()`, use `writeSegment()` if the column has been initialized already,
   * and `insertSegment()` otherwise.
   */
  template <typename T>
  void writeSegment(FileMemSegments rows, const Column<T>& column) const;

  /// @}
  /**
   * @name Write a sequence of columns.
   */
  /// @{

  /**
   * @brief Write several columns.
   */
  template <typename TSeq>
  void writeSeq(TSeq&& columns) const;

  /**
   * @brief Write several columns.
   * @copydetails writeSeq()
   */
  template <typename... Ts>
  void writeSeq(const Column<Ts>&... columns) const;

  /**
   * @brief Append or insert a sequence of columns, which were not previously initialized.
   * @param index The 0-based index of the first column to be added, which may be >= 0 or -1 to append the columns at the end
   * @param info The column infos
   */
  template <typename TSeq>
  void initSeq(long index, TSeq&& infos) const;

  /**
   * @copydoc initSeq
   */
  template <typename... Ts>
  void initSeq(long index, const ColumnInfo<Ts>&... infos) const;

  /**
   * @brief Remove a sequence of columns specified by their names or indices.
   */
  void removeSeq(std::vector<ColumnKey> keys) const;

  /// @}
  /**
   * @name Write a sequence of column segments.
   */
  /// @{

  /**
   * @brief Write a sequence of segments.
   * @param rows The mapping between the in-file and in-memory rows
   * @param columns The columns to be written
   * Segments can be written in already initialized columns with `writeSegmentSeq()`
   * or in new columns with `appendSegmentSeq()`.
   */
  template <typename TSeq>
  void writeSegmentSeq(FileMemSegments rows, TSeq&& columns) const;

  /**
   * @copydoc writeSegmentSeq
   */
  template <typename... Ts>
  void writeSegmentSeq(FileMemSegments rows, const Column<Ts>&... columns) const;

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
 * @brief The common number of rows of a sequence of columns.
 * @details
 * Throw if columns do not have the same size.
 */
template <typename TSeq>
long columnsRowCount(TSeq&& columns);

} // namespace Fits
} // namespace Euclid

/// @cond INTERNAL
#define _ELEFITS_BINTABLECOLUMNS_IMPL
#include "EleFits/impl/BintableColumns.hpp"
#undef _ELEFITS_BINTABLECOLUMNS_IMPL
/// @endcond

#endif
