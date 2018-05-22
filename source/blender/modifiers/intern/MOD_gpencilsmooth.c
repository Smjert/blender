/*
 * ***** BEGIN GPL LICENSE BLOCK *****
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software  Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The Original Code is Copyright (C) 2017, Blender Foundation
 * This is a new part of Blender
 *
 * Contributor(s): Antonio Vazquez
 *
 * ***** END GPL LICENSE BLOCK *****
 *
 */

/** \file blender/modifiers/intern/MOD_gpencilsmooth.c
 *  \ingroup modifiers
 */

#include <stdio.h>

#include "DNA_meshdata_types.h"
#include "DNA_object_types.h"
#include "DNA_gpencil_types.h"
#include "DNA_modifier_types.h"

#include "BKE_context.h"
#include "BKE_deform.h"
#include "BKE_gpencil.h"

#include "DEG_depsgraph.h"

#include "MOD_modifiertypes.h"
#include "MOD_gpencil_util.h"

static void initData(ModifierData *md)
{
	SmoothGpencilModifierData *gpmd = (SmoothGpencilModifierData *)md;
	gpmd->pass_index = 0;
	gpmd->flag |= GP_SMOOTH_MOD_LOCATION;
	gpmd->factor = 0.5f;
	gpmd->layername[0] = '\0';
	gpmd->vgname[0] = '\0';
	gpmd->step = 1;
}

static void copyData(const ModifierData *md, ModifierData *target)
{
	modifier_copyData_generic(md, target);
}

/* aply smooth effect based on stroke direction */
static void gp_deformStroke(
        ModifierData *md, Depsgraph *UNUSED(depsgraph),
        Object *ob, bGPDlayer *gpl, bGPDstroke *gps)
{
	SmoothGpencilModifierData *mmd = (SmoothGpencilModifierData *)md;
	int vindex = defgroup_name_index(ob, mmd->vgname);
	float weight = 1.0f;
	float val;

	if (!is_stroke_affected_by_modifier(ob,
	        mmd->layername, mmd->pass_index, 3, gpl, gps,
	        mmd->flag & GP_SMOOTH_INVERT_LAYER, mmd->flag & GP_SMOOTH_INVERT_PASS))
	{
		return;
	}

	/* smooth stroke */
	if (mmd->factor > 0.0f) {
		for (int r = 0; r < mmd->step; r++) {
			for (int i = 0; i < gps->totpoints; i++) {
				// bGPDspoint *pt = &gps->points[i];
				MDeformVert *dvert = &gps->dvert[i];

				/* verify vertex group */
				weight = get_modifier_point_weight(dvert, (int)(!(mmd->flag & GP_SMOOTH_INVERT_VGROUP) == 0), vindex);
				if (weight < 0) {
					continue;
				}

				val = mmd->factor * weight;
				/* perform smoothing */
				if (mmd->flag & GP_SMOOTH_MOD_LOCATION) {
					BKE_gpencil_smooth_stroke(gps, i, val);
				}
				if (mmd->flag & GP_SMOOTH_MOD_STRENGTH) {
					BKE_gpencil_smooth_stroke_strength(gps, i, val);
				}
				if ((mmd->flag & GP_SMOOTH_MOD_THICKNESS)  && (val > 0)) {
					/* thickness need to repeat process several times */
					for (int r2 = 0; r2 < r * 10; r2++) {
						BKE_gpencil_smooth_stroke_thickness(gps, i, val);
					}
				}
				if (mmd->flag & GP_SMOOTH_MOD_UV) {
					BKE_gpencil_smooth_stroke_uv(gps, i, val);
				}
			}
		}
	}
}

static void gp_bakeModifier(
        const bContext *UNUSED(C), Depsgraph *depsgraph,
        ModifierData *md, Object *ob)
{
	bGPdata *gpd = ob->data;

	for (bGPDlayer *gpl = gpd->layers.first; gpl; gpl = gpl->next) {
		for (bGPDframe *gpf = gpl->frames.first; gpf; gpf = gpf->next) {
			for (bGPDstroke *gps = gpf->strokes.first; gps; gps = gps->next) {
				gp_deformStroke(md, depsgraph, ob, gpl, gps);
			}
		}
	}
}

ModifierTypeInfo modifierType_Gpencil_Smooth = {
	/* name */              "Smooth",
	/* structName */        "SmoothGpencilModifierData",
	/* structSize */        sizeof(SmoothGpencilModifierData),
	/* type */              eModifierTypeType_Gpencil,
	/* flags */             eModifierTypeFlag_GpencilMod | eModifierTypeFlag_SupportsEditmode,

	/* copyData */          copyData,

	/* deformVerts_DM */    NULL,
	/* deformMatrices_DM */ NULL,
	/* deformVertsEM_DM */  NULL,
	/* deformMatricesEM_DM*/NULL,
	/* applyModifier_DM */  NULL,
	/* applyModifierEM_DM */NULL,

	/* deformVerts */       NULL,
	/* deformMatrices */    NULL,
	/* deformVertsEM */     NULL,
	/* deformMatricesEM */  NULL,
	/* applyModifier */     NULL,
	/* applyModifierEM */   NULL,

	/* gp_deformStroke */      gp_deformStroke,
	/* gp_generateStrokes */   NULL,
	/* gp_bakeModifier */    gp_bakeModifier,

	/* initData */          initData,
	/* requiredDataMask */  NULL,
	/* freeData */          NULL,
	/* isDisabled */        NULL,
	/* updateDepsgraph */   NULL,
	/* dependsOnTime */     NULL,
	/* dependsOnNormals */	NULL,
	/* foreachObjectLink */ NULL,
	/* foreachIDLink */     NULL,
	/* foreachTexLink */    NULL,
};
