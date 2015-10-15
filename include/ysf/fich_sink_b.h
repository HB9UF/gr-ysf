/* -*- c++ -*- */
/* 
 * Copyright 2015 Mathias Weyland
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */


#ifndef INCLUDED_YSF_FICH_SINK_C_H
#define INCLUDED_YSF_FICH_SINK_C_H

#include <ysf/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace ysf {

    /*!
     * \brief <+description of block+>
     * \ingroup ysf
     *
     */
    class YSF_API fich_sink_b : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<fich_sink_b> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of ysf::fich_sink_b.
       *
       * To avoid accidental use of raw pointers, ysf::fich_sink_b's
       * constructor is in a private implementation
       * class. ysf::fich_sink_b::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace ysf
} // namespace gr

#endif /* INCLUDED_YSF_FICH_SINK_C_H */

