/*************************************************************************
 *
 * This file is part of the SAMRAI distribution.  For full copyright
 * information, see COPYRIGHT and COPYING.LESSER.
 *
 * Copyright:     (c) 1997-2013 Lawrence Livermore National Security, LLC
 * Description:   Communication transaction for time interpolation during data
 *                refining
 *
 ************************************************************************/

#ifndef included_xfer_RefineTimeTransaction
#define included_xfer_RefineTimeTransaction

#include "SAMRAI/SAMRAI_config.h"

#include "SAMRAI/tbox/Transaction.h"
#include "SAMRAI/tbox/Utilities.h"
#include "SAMRAI/hier/BaseGridGeometry.h"
#include "SAMRAI/hier/PatchLevel.h"
#include "SAMRAI/xfer/RefineClasses.h"

#include <iostream>

namespace SAMRAI {
namespace xfer {

/*!
 * @brief Class RefineTimeTransaction represents a single time interpolation
 * communication transaction between two processors or a local data copy or
 * refine schedules.  Note that to there is an implicit hand-shaking between
 * objects of this class and the RefineSchedule object that constructs them.
 * Following the refine schedule implementation, the source patch data indices
 * for a time transaction are always refer to the old and new source data and
 * the destination patch data index for a time transaction is always the
 * scratch data, all as defined in the RefineClasses class.  This transaction
 * is used by the refine schedule.
 *
 * @see xfer::RefineSchedule
 * @see xfer::RefineClasses
 * @see tbox::Schedule
 * @see tbox::Transaction
 */

class RefineTimeTransaction:public tbox::Transaction
{
public:
   /*!
    * Static member function to set the array of refine class data items that
    * is shared by all object instances of this time transaction class during
    * data transfers.  The array must be set before any transactions are
    * executed.  The array is set in the RefineSchedule class.
    *
    * @pre refine_items != 0
    * @pre num_refine_items >= 0
    */
   static void
   setRefineItems(
      const RefineClasses::Data** refine_items,
      int num_refine_items)
   {
      TBOX_ASSERT(refine_items != 0);
      TBOX_ASSERT(num_refine_items >= 0);
      s_refine_items = refine_items;
      s_num_refine_items = num_refine_items;
   }

   /*!
    * Static member function to unset the array of refine class data items that
    * is shared by all object instances of this time transaction class during
    * data transfers.  The unset function is used to prevent erroneous
    * execution of different schedules.  The array is unset in the
    * RefineSchedule class.
    */
   static void
   unsetRefineItems()
   {
      s_refine_items = 0;
      s_num_refine_items = 0;
   }

   /*!
    * Static member function to set the transaction time that will be shared
    * by all object instances of this time transaction class during time
    * interpolation.  This transaction time must be set before any transactions
    * are executed.
    */
   static void
   setTransactionTime(
      const double time)
   {
      s_time = time;
   }

   /*!
    * Construct a transaction with the specified source and destination levels,
    * patches, and patch data components found in the refine class
    * item with the given id owned by the calling refine schedule.  In general,
    * this constructor is called by a RefineSchedule object for each data
    * transaction (involving time interpolation) that must occur.  This
    * transaction will be responsible for one of the following: (1) performing
    * a local copy, (2) packing a message stream from the source, or
    * (3) unpacking a message stream from the destination.  The transaction
    * will perform time interpolation between the source old and new times
    * using the time interpolation operator found in the refine class item.
    *
    * @param dst_level      boost::shared_ptr to destination patch level.
    * @param src_level      boost::shared_ptr to source patch level.
    * @param overlap        boost::shared_ptr to overlap region between
    *                       patches.
    * @param dst_box        Destination Box in destination patch level.
    * @param src_box        Source Box in source patch level.
    * @param box            hier::Box region in which to time interpolate.
    * @param refine_item_id   Integer id of refine data item owned by refine
    *                         schedule.
    *
    * @pre dst_level
    * @pre src_level
    * @pre overlap
    * @pre dst_box.getLocalId() >= 0
    * @pre src_box.getLocalId() >= 0
    * @pre refine_item_id >= 0
    * @pre (dst_level->getDim() == src_level->getDim()) &&
    *      (dst_level->getDim() == dst_box.getDim()) &&
    *      (dst_level->getDim() == src_box.getDim()) &&
    *      (dst_level->getDim() == box.getDim())
    */
   RefineTimeTransaction(
      const boost::shared_ptr<hier::PatchLevel>& dst_level,
      const boost::shared_ptr<hier::PatchLevel>& src_level,
      const boost::shared_ptr<hier::BoxOverlap>& overlap,
      const hier::Box& dst_box,
      const hier::Box& src_box,
      const hier::Box& box,
      const int refine_item_id);

   /*!
    * The virtual destructor for time transaction releases all
    * memory associated with the transaction.
    */
   virtual ~RefineTimeTransaction();

   /*!
    * Return a boolean indicating whether this transaction can estimate
    * the size of an incoming message.  If this is false, then a different
    * communications protocol kicks in and the message size is transmitted
    * between boxes.
    */
   virtual bool
   canEstimateIncomingMessageSize();

   /*!
    * Return the integer amount of buffer space (in bytes) needed for the
    * incoming message.  This routine is only called if the transaction
    * can estimate the size of the incoming message.
    */
   virtual size_t
   computeIncomingMessageSize();

   /*!
    * Return the integer buffer space needed (in bytes) for the outgoing
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
   RefineTimeTransaction(
      const RefineTimeTransaction&);                    // not implemented
   void
   operator = (
      const RefineTimeTransaction&);                    // not implemented

   static double s_time;
   static const RefineClasses::Data** s_refine_items;
   static int s_num_refine_items;

   void
   timeInterpolate(
      const boost::shared_ptr<hier::PatchData>& pd_dst,
      const boost::shared_ptr<hier::PatchData>& pd_old,
      const boost::shared_ptr<hier::PatchData>& pd_new);

   boost::shared_ptr<hier::Patch> d_dst_patch;
   int d_dst_patch_rank;
   boost::shared_ptr<hier::Patch> d_src_patch;
   int d_src_patch_rank;
   boost::shared_ptr<hier::BoxOverlap> d_overlap;
   hier::Box d_box;
   int d_refine_item_id;
   int d_incoming_bytes;
   int d_outgoing_bytes;

};

}
}

#endif