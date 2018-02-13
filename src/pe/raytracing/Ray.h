//======================================================================================================================
//
//  This file is part of waLBerla. waLBerla is free software: you can
//  redistribute it and/or modify it under the terms of the GNU General Public
//  License as published by the Free Software Foundation, either version 3 of
//  the License, or (at your option) any later version.
//
//  waLBerla is distributed in the hope that it will be useful, but WITHOUT
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//  for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with waLBerla (see COPYING.txt). If not, see <http://www.gnu.org/licenses/>.
//
//! \file Ray.h
//! \author Lukas Werner
//
//======================================================================================================================

#pragma once

#include "core/DataTypes.h"
#include "core/math/Vector3.h"
#include <pe/rigidbody/RigidBody.h>

namespace walberla {
namespace pe {
namespace raytracing {
   
class Ray {
public:
   /*!\name Constructors */
   //@{
   /*!\brief Instantiation constructor for the Raytracer class.
    */
   Ray () {
      Ray (Vec3(0,0,0), Vec3(1,0,0));
   }
   
   /*!\brief Instantiation constructor for the Raytracer class.
    * \param origin Origin of the ray. ()
    * \param direction Normalized direction of the ray.
    */
   Ray (Vec3 origin, Vec3 direction) {
      setDirection(direction);
      setOrigin(origin);
   }
   //@}

private:
   /*!\name Member variables */
   //@{
   Vec3 origin_; //!< Origin of the ray.
   Vec3 direction_; //!< The normalized direction of the ray.
   Vec3 inv_direction_; //!< The inverted direction of the ray.
   Vector3<int8_t> sign_; /*!< The signs of the inverted direction of the ray.
                           (Required for Ray-Box intersection code.)*/
   //@}

public:
   /*!\name Get functions */
   //@{
   /*!\brief Returns the origin point of the ray.
    */
   inline const Vec3& getOrigin () const {
      return origin_;
   }
   
   /*!\brief Returns the normalized direction vector of the ray.
    */
   inline const Vec3& getDirection () const {
      return direction_;
   }
   
   /*!\brief Returns the inverse of the direction vector of the ray.
    */
   inline const Vec3& getInvDirection () const {
      return inv_direction_;
   }
   
   /*!\brief Returns the signs of the inverted direction vector of the ray.
    *
    * Returns the signs of the inverted direction vector of the ray as required for the ray-box intersection algorithm.
    */
   inline const Vector3<int8_t>& getInvDirectionSigns () const {
      return sign_;
   }
   //@}

   /*!\name Set functions */
   //@{
   /*!\brief Set the origin point of the ray.
    * \param origin Origin point
    */
   inline void setOrigin (const Vec3& origin) {
      origin_ = origin;
   }
   
   /*!\brief Set the _normalized_ direction vector of the ray.
    * \param direction Normalized direction vector
    */
   inline void setDirection (const Vec3& direction) {
      // im kommentar verweis auf normalisierung
      WALBERLA_CHECK_FLOAT_EQUAL(direction.length(), real_t(1));
      direction_ = direction;
      calcInvDirection();
   }
   //@}

   /*!\name Utility functions */
   //@{
   /*!\brief Calculates the inverse of the direction vector and saves its signs.
    */
   inline void calcInvDirection () {
      inv_direction_ = Vec3(1/direction_[0], 1/direction_[1], 1/direction_[2]);
      sign_[0] = int8_c(inv_direction_[0] < 0);
      sign_[1] = int8_c(inv_direction_[1] < 0);
      sign_[2] = int8_c(inv_direction_[2] < 0);
   }
   
   /*!\brief Transforms the ray to the body frame.
    *
    * \return Ray transformed to the body frame.
    */
   Ray transformedToBF(const BodyID body) const {
      return Ray(body->pointFromWFtoBF(getOrigin()), body->vectorFromWFtoBF(getDirection()));
   }
   //@}
};

}
}
}
