#include "api.h"
#include <svn_client.h>
#include <svn_cmdline.h>
#include <svn_pools.h>
#include <oak/oak.h>
#include <oak/debug.h>

OAK_DEBUG_VAR(SCM_Subversion);

static scm::status::type type_for_svn_status (svn_wc_status_kind status)
{
	switch(status)
	{
		case svn_wc_status_modified:     return scm::status::modified;
		case svn_wc_status_added:        return scm::status::added;
		case svn_wc_status_missing:
		case svn_wc_status_deleted:      return scm::status::deleted;
		case svn_wc_status_conflicted:   return scm::status::conflicted;
		case svn_wc_status_normal:       return scm::status::versioned;
		case svn_wc_status_ignored:      return scm::status::ignored;
		default:                         return scm::status::unversioned;
	}
}

static svn_error_t* svn_status_func (void* baton, char const* path, svn_wc_status2_t* status, apr_pool_t* scratch_pool)
{
	scm::status_map_t& statusMap = *(scm::status_map_t*)baton;
	statusMap.insert(std::make_pair(path, type_for_svn_status(status->text_status)));
	return NULL;
}

static scm::status_map_t status_for_path (std::string const& path, svn_client_ctx_t* context)
{
	D(DBF_SCM_Subversion, bug("%s\n", path.c_str()););
	scm::status_map_t statusMap;
	if(apr_pool_t* pool = svn_pool_create(NULL))
	{
		svn_client_status4(NULL, path.c_str(), NULL, svn_status_func, &statusMap, svn_depth_infinity, TRUE, FALSE, TRUE, TRUE, NULL, context, pool); // we use svn_depth_infinity since ‘files_with_status’ needs to include all files in the repository, but technically each folder is its own repository, and for normal operation, it’s fine to use svn_depth_immediates
		svn_pool_destroy(pool);
	}
	return statusMap;
}

namespace scm
{
	struct svn_driver_t : driver_t
	{
		svn_driver_t () : driver_t("svn", "%s/.svn")
		{
			svn_cmdline_init(getprogname(), stderr);
			_pool = svn_pool_create(NULL);
			svn_client_create_context(&_context, _pool);
		}

		std::string branch_name (std::string const& wcPath) const { return NULL_STR;                              }
		status_map_t status (std::string const& wcPath) const     { return status_for_path(wcPath, _context);     }

	private:
		apr_pool_t* _pool;
		svn_client_ctx_t* _context;
	};

	driver_t* svn_driver () { return new svn_driver_t; }
}
