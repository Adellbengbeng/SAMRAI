/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright
 * information, see COPYRIGHT and COPYING.LESSER.
 *
 * Copyright:     (c) 1997-2013 Lawrence Livermore National Security, LLC
 * Description:   Communication transaction for data copies during data
 *                coarsening
 *
 ************************************************************************/

#ifndef included_xfer_CoarsenCopyTransaction
#define included_xfer_CoarsenCopyTransaction

#include "SAMRAI/SAMRAI_config.h"

#include "SAMRAI/tbox/Transaction.h"
#include "SAMRAI/hier/BaseGridGeometry.h"
#include "SAMRAI/hier/PatchLevel.h"
#include "SAMRAI/xfer/CoarsenClasses.h"

#include <iostream>

namespace SAMRAI {
namespace xfer {

/*!
 * @brief Class CoarsenCopyTransaction represents a single copy communication
 * transaction between two processors or a local data copy for coaren
 * schedules.  Note that to there is an implicit hand-shaking between objects
 * of this class and the CoarsenSchedule object that constructs them.
 * Following the coarsen schedule implementation, the source patch data index
 * for a copy transaction always refers to the source data, and the destination
 * patch data index for a copy transaction is always the destination data, all
 * as defined in the CoarsenClasses class.
 *
 * @see xfer::CoarsenSchedule
 * @see xfer::CoarsenClasses
 * @see tbox::Schedule
 * @see tbox::Transaction
 */

class CoarsenCopyTransaction:public tbox::Transaction
{
public:
   /*!
    * Static member function to set the array of coarsen class data items that
    * is shared by all object instances of this copy transaction class during
    * data transfers.  The array must be set before any transactions are
    * executed.  The array is set in the CoarsenSchedule class.
    *
    * @pre coarsen_items != 0
    * @pre num_coarsen_items >= 0
    */
   static void
   setCoarsenItems(
      const CoarsenClasses::Data** coarsen_items,
      int num_coarsen_items)
   {
      TBOX_ASSERT(coarsen_items != 0);
      TBOX_ASSERT(num_coarsen_items >= 0);
      s_coarsen_items = coarsen_items;
      s_num_coarsen_items = num_coarsen_items;
   }

   /*!
    * Static member function to unset the array of coarsen class data items
    * that is shared by all object instances of this copy transaction class
    * during data transfers.  The unset function is used to prevent erroneous
    * execution of different schedules.  The array is unset in the
    * CoarsenSchedule class.
    */
   static void
   unsetCoarsenItems()
   {
      s_coarsen_items = 0;
      s_num_coarsen_items = 0;
   }

   /*!
    * Construct a transaction with the specified source and destination
    * levels, patches, and patch data components found in the coarsen class
    * item with the given id owned by the calling coarsen schedule.  In
    * general, this constructor is called by a CoarsenSchedule object for each
    * data transaction (not involving time interpolation) that must occur.
    * This transaction will be responsible for one of the following: (1) a
    * local data copy, (2) packing a message stream with source patch data, or
    * (3) unpacking destination patch data from a message stream.
    *
    * @param dst_level        boost::shared_ptr to destination patch level.
    * @param src_level        boost::shared_ptr to source patch level.
    * @param overlap          boost::shared_ptr to overlap region between
    *                         patches.
    * @param dst_box          Destination Box in destination patch level.
    * @param src_box          Source Box in source patch level.
    * @param coarsen_item_id  Integer id of coarsen data item owned by coarsen
    *                         schedule.
    *
    * @pre dst_level
    * @pre src_level
    * @pre overlap
    * @pre (dst_level->getDim() == src_level->getDim()) &&
    *      (dst_level->getDim() == dst_box.getDim()) &&
    *      (dst_level->getDim() == src_box.getDim())
    * @pre dst_box.getLocalId() >= 0
    * @pre src_box.getLocalId() >= 0
    * @pre coarsen_item_id >= 0
    */
   CoarsenCopyTransaction(
      const boost::shared_ptr<hier::PatchLevel>& dst_level,
      const boost::shared_ptr<hier::PatchLevel>& src_level,
      const boost::shared_ptr<hier::BoxOverlap>& overlap,
      const hier::Box& dst_box,
      const hier::Box& src_box,
      const int coarsen_item_id);

   /*!
    * The virtual destructor for the copy transaction releases all
    * memory associated with the transaction.
    */
   virtual ~CoarsenCopyTransaction();

   /*!
    * Return a boolean indicating whether this transaction can estimate
    * the size of an incoming message.  If this is false, then a different
    * communication protocol kicks in and the message size is transmitted
    * between boxes.
    */
   virtual bool
   canEstimateIncomingMessageSize();

   /*!
    * Return the integer buffer space (in bytes) needed for the incoming
    * message.  This routine is only called if the transaction can estimate the
    * size of the incoming message.  See canEstimateIncomingMessageSize().
    */
   virtual size_t
   computeIncomingMessageSize();

   /*!
    * Return the integer buffer space (in bytes) needed for the outgoing
    * message.
    */
   virtual size_t
   computeOutgoingMessageSize();

   /*!
    * Return the sending processor number for the communications transaction.
    */
   virtual int
   getSourceProcessor();

   /*!
    * Return the receiving processor number for the communications transaction.
    */
   virtual int
   getDestinationProcessor();

   /*!
    * Pack the transaction data into the message stream.
    */
   virtual void
   packStream(
      tbox::MessageStream& stream);

   /*!
    * Unpack the transaction data from the message stream.
    */
   virtual void
   unpackStream(
      tbox::MessageStream& stream);

   /*!
    * Perform the local data copy for the transaction.
    */
   virtual void
   copyLocalData();

   /*!
    * Print out transaction information.
    */
   virtual void
   printClassData(
      std::ostream& stream) const;

private:
   CoarsenCopyTransaction(
      const CoarsenCopyTransaction&);                     // not implemented
   void
   operator = (
      const CoarsenCopyTransaction&);                   // not implemented

   static const CoarsenClasses::Data** s_coarsen_items;
   static int s_num_coarsen_items;

   boost::shared_ptr<hier::Patch> d_dst_patch;
   int d_dst_patch_rank;
   boost::shared_ptr<hier::Patch> d_src_patch;
   int d_src_patch_rank;
   boost::shared_ptr<hier::BoxOverlap> d_overlap;
   int d_coarsen_item_id;
   int d_incoming_bytes;
   int d_outgoing_bytes;

};

}
}

#endif