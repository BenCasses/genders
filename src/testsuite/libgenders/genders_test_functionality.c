/*****************************************************************************\
 *  $Id: genders_test_functionality.c,v 1.3 2005-01-03 17:31:21 achu Exp $
 *****************************************************************************
 *  Copyright (C) 2001-2003 The Regents of the University of California.
 *  Produced at Lawrence Livermore National Laboratory (cf, DISCLAIMER).
 *  Written by Jim Garlick <garlick@llnl.gov> and Albert Chu <chu11@llnl.gov>.
 *  UCRL-CODE-2003-004.
 *
 *  This file is part of Genders, a cluster configuration database.
 *  For details, see <http://www.llnl.gov/linux/genders/>.
 *
 *  Genders is free software; you can redistribute it and/or modify it under
 *  the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  Genders is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with Genders; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place, Suite 330, Boston, MA  02111-1307  USA.
\*****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <paths.h>		/* _PATH_DEVNULL */
#include <unistd.h>		/* gethostname */
#include <sys/param.h>		/* MAXHOSTNAMELEN */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>       	/* stat() */
#include <errno.h>
#include <assert.h>

#include "genders.h"
#include "genders_testlib.h"
#include "genders_test_functionality.h"
#include "genders_test_database.h"
#include "genders_test_query_tests.h"

int
genders_handle_create_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int err, num = 0;

  /* Test 0: Successfully create handle */

  handle = genders_handle_create();

  err = genders_pointer_exist_check("genders_handle_create",
				    num,
				    handle,
				    "handle",
				    GENDERS_POINTER_NON_NULL,
				    NULL,
				    verbose);
  errcount += err;
  num++;

  if (handle)
    {
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
    }

  return errcount;
}

int
genders_handle_destroy_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int return_value, errnum, err;

  /* Test 0: Successfully destroy unloaded handle */

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");

  return_value = genders_handle_destroy(handle);
  errnum = genders_errnum(handle);
  
  err = genders_return_value_errnum_check("genders_handle_destroy",
					  num,
					  0,
					  GENDERS_ERR_MAGIC,
					  return_value,
					  errnum,
					  NULL,
					  verbose);
  if (err)
      /* Don't exit on error, just make a best effort */
      genders_handle_destroy(handle);

  errcount += err;
  num++;

  /* Test 1: Successfully destroy loaded handle */

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");

  if (genders_load_data(handle, genders_database_base.filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));

  return_value = genders_handle_destroy(handle);
  errnum = genders_errnum(handle);
  
  err = genders_return_value_errnum_check("genders_handle_destroy",
					  num,
					  0,
					  GENDERS_ERR_MAGIC,
					  return_value,
					  errnum,
					  NULL,
					  verbose);
  if (err)
      /* Don't exit on error, just make a best effort */
      genders_handle_destroy(handle);

  errcount += err;
  num++;

  return errcount;
}

int
genders_load_data_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;

  /* Part A: Successfully find parse errors */
  {
    int i = 0;
    genders_parse_error_database_t *databases = &genders_parse_error_databases[0];

    while (databases[i].filename != NULL)
      {
	genders_t handle;
	int return_value, errnum, err;

	if (!(handle = genders_handle_create()))
	  genders_err_exit("genders_handle_create");

	return_value = genders_load_data(handle, databases[i].filename);
	errnum = genders_errnum(handle);

	err = genders_return_value_errnum_check("genders_load_data",
						num,
						-1,
						GENDERS_ERR_PARSE,
						return_value,
						errnum,
						databases[i].filename,
						verbose);

	if (genders_handle_destroy(handle) < 0)
	  genders_err_exit("genders_handle_create");

	errcount += err;
	num++;
	i++;
      }
  }
  
  /* Part B: Load functionality test databases without errors */
  {
    int i = 0;
    genders_database_t **databases = &genders_functionality_databases[0];

    while (databases[i] != NULL)
      {
	genders_t handle;
	int return_value, errnum, err;

	if (!(handle = genders_handle_create()))
	  genders_err_exit("genders_handle_create");

	return_value = genders_load_data(handle, databases[i]->filename);
	errnum = genders_errnum(handle);

	err = genders_return_value_errnum_check("genders_load_data",
						num,
						0,
						GENDERS_ERR_SUCCESS,
						return_value,
						errnum,
						databases[i]->filename,
						verbose);
	
	if (genders_handle_destroy(handle) < 0)
	  genders_err_exit("genders_handle_create");

	errcount += err;
	num++;
	i++;
      }
  }

  return errcount;
}

int
genders_errnum_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");
  
  if (genders_load_data(handle, genders_database_base.filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
  for (i = GENDERS_ERR_SUCCESS; i <= GENDERS_ERR_ERRNUMRANGE; i++)
    {
      int errnum, err;

      genders_set_errnum(handle, i);
      errnum = genders_errnum(handle);
	
      err = genders_errnum_check("genders_errnum",
				 num,
				 i,
				 errnum,
				 NULL,
				 verbose);
      errcount += err;
      num++;
    }

  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_create");

  return errcount;
}

int
genders_strerror_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");
  
  if (genders_load_data(handle, genders_database_base.filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
  for (i = GENDERS_ERR_SUCCESS; i <= GENDERS_ERR_ERRNUMRANGE; i++)
    {
      char *expected_string, *string;
      int err;

      genders_set_errnum(handle, i);
      expected_string = genders_errormsg(handle);
      string = genders_strerror(i);
	
      err = genders_string_check("genders_strerror",
				 num,
				 expected_string,
				 "expected_string",
				 string,
				 "string",
				 NULL,
				 verbose);

      errcount += err;
      num++;
    }

  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_create");

  return errcount;
}

int
genders_errormsg_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");
  
  if (genders_load_data(handle, genders_database_base.filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
  for (i = GENDERS_ERR_SUCCESS; i <= GENDERS_ERR_ERRNUMRANGE; i++)
    {
      char *expected_string, *string;
      int err;

      expected_string = genders_strerror(i);
      genders_set_errnum(handle, i);
      string = genders_errormsg(handle);
	
      err = genders_string_check("genders_errormsg",
				 num,
				 expected_string,
				 "expected_string",
				 string,
				 "string",
				 NULL,
				 verbose);

      errcount += err;
      num++;
    }

  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_create");

  return errcount;
}

int
genders_perror_functionality(int verbose)
{
  genders_t handle;
  FILE *stderr_save;
  int errcount = 0;
  int num = 0;
  int i = 0;

  stderr_save = stderr;

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");
  
  if (genders_load_data(handle, genders_database_base.filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
  for (i = GENDERS_ERR_SUCCESS; i <= GENDERS_ERR_ERRNUMRANGE; i++)
    {
      char *expected_string;
      char expected_string_buf[GENDERS_ERR_BUFLEN];
      char string_buf[GENDERS_ERR_BUFLEN];
      int len, err;
      int fds[2];
      FILE *in, *out;

      if (pipe(fds) < 0)
	genders_err_exit("pipe: %s", strerror(errno));
      
      if (!(in = fdopen(fds[0], "r")))
	genders_err_exit("fdopen: %s", strerror(errno));
      
      if (!(out = fdopen(fds[1], "w")))
	genders_err_exit("fdopen2: %s", strerror(errno));

      genders_set_errnum(handle, i);
      expected_string = genders_strerror(i);

      stderr = out;
      genders_perror(handle, NULL);
      stderr = stderr_save;
      fclose(out);		/* so read won't block */

      if ((len = read(fds[0], string_buf, GENDERS_ERR_BUFLEN)) < 0)
	genders_err_exit("read: %s", strerror(errno));
      string_buf[len] = '\0';
      fclose(in);

      strcpy(expected_string_buf, expected_string);
      strcat(expected_string_buf, "\n");

      err = genders_string_check("genders_perror",
				 num,
				 expected_string_buf,
				 "expected_string",
				 string_buf,
				 "string",
				 NULL,
				 verbose);
      errcount += err;
      num++;
    }

  for (i = GENDERS_ERR_SUCCESS; i <= GENDERS_ERR_ERRNUMRANGE; i++)
    {
      char *expected_string, *msg = "foo";
      char expected_string_buf[GENDERS_ERR_BUFLEN];
      char string_buf[GENDERS_ERR_BUFLEN];
      int len, err;
      int fds[2];
      FILE *in, *out;

      if (pipe(fds) < 0)
	genders_err_exit("pipe: %s", strerror(errno));
      
      if (!(in = fdopen(fds[0], "r")))
	genders_err_exit("fdopen: %s", strerror(errno));
      
      if (!(out = fdopen(fds[1], "w")))
	genders_err_exit("fdopen2: %s", strerror(errno));

      genders_set_errnum(handle, i);
      expected_string = genders_strerror(i);

      stderr = out;
      genders_perror(handle, msg);
      stderr = stderr_save;
      fclose(out);		/* so read won't block */

      if ((len = read(fds[0], string_buf, GENDERS_ERR_BUFLEN)) < 0)
	genders_err_exit("read: %s", strerror(errno));
      string_buf[len] = '\0';
      fclose(in);

      strcpy(expected_string_buf, msg);
      strcat(expected_string_buf, ": ");
      strcat(expected_string_buf, expected_string);
      strcat(expected_string_buf, "\n");

      err = genders_string_check("genders_perror",
				 num,
				 expected_string_buf,
				 "expected_string",
				 string_buf,
				 "string",
				 NULL,
				 verbose);
      errcount += err;
      num++;
    }

  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_create");
  return errcount;
}

static int
_genders_getfunc_functionality(GendersGetFunc func, 
			       char *funcname,
			       char *filename,
			       int num,
			       int expected_return_value,
			       int expected_errnum,
			       char *msg,
			       int verbose)
{
  genders_t handle;
  int return_value, errnum, err;
	
  assert(func && funcname && filename);

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");

  if (genders_load_data(handle, filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));

  return_value = func(handle);
  errnum = genders_errnum(handle);
		 
  err = genders_return_value_errnum_check(funcname,
					  num,
					  expected_return_value,
					  expected_errnum,
					  return_value,
					  errnum,
					  msg,
					  verbose);

  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_create");

  return err;
}

int
genders_getnumnodes_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_getfunc_functionality(genders_getnumnodes,
					   "genders_getnumnodes",
					   databases[i]->filename,
					   num, 
					   databases[i]->data->numnodes,
					   GENDERS_ERR_SUCCESS,
					   databases[i]->filename,
					   verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_getnumattrs_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_getfunc_functionality(genders_getnumattrs,
					   "genders_getnumattrs",
					   databases[i]->filename,
					   num, 
					   databases[i]->data->numattrs,
					   GENDERS_ERR_SUCCESS,
					   databases[i]->filename,
					   verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_getmaxattrs_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_getfunc_functionality(genders_getmaxattrs,
					   "genders_getmaxattrs",
					   databases[i]->filename,
					   num, 
					   databases[i]->data->maxattrs,
					   GENDERS_ERR_SUCCESS,
					   databases[i]->filename,
					   verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_getmaxnodelen_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];
  int hostnamelen;

  /* Special case.  If you are on a host whose hostname length is
   * greater than the database maxnodelen, then that is the actual
   * maxnodelen.
   */
  {
    char buffer[MAXHOSTNAMELEN+1];
    char *ptr;

    memset(buffer, '\0', MAXHOSTNAMELEN+1);

    if (gethostname(buffer, MAXHOSTNAMELEN) < 0)
      genders_err_exit("gethostname: %s", strerror(errno));

    /* shorten if necessary */
    if ((ptr = strchr(buffer,'.')))
      *ptr = '\0';

    hostnamelen = strlen(buffer);
  }

  while (databases[i] != NULL)
    {
      int err, maxnodelen;
      
      if (hostnamelen > databases[i]->data->maxnodelen)
	maxnodelen = hostnamelen;
      else
	maxnodelen = databases[i]->data->maxnodelen;
      err = _genders_getfunc_functionality(genders_getmaxnodelen,
					   "genders_getmaxnodelen",
					   databases[i]->filename,
					   num, 
					   maxnodelen,
					   GENDERS_ERR_SUCCESS,
					   databases[i]->filename,
					   verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_getmaxattrlen_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_getfunc_functionality(genders_getmaxattrlen,
					   "genders_getmaxattrlen",
					   databases[i]->filename,
					   num, 
					   databases[i]->data->maxattrlen,
					   GENDERS_ERR_SUCCESS,
					   databases[i]->filename,
					   verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_getmaxvallen_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_getfunc_functionality(genders_getmaxvallen,
					   "genders_getmaxvallen",
					   databases[i]->filename,
					   num, 
					   databases[i]->data->maxvallen,
					   GENDERS_ERR_SUCCESS,
					   databases[i]->filename,
					   verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

static int
_genders_list_create_functionality(GendersListCreateFunc funccreate,
				   GendersListDestroyFunc funcdestroy,
				   char *funcname,
				   char *filename,
				   int num,
				   int expected_return_value,
				   int expected_errnum,
				   char *msg,
				   int verbose)
{
  genders_t handle;
  char **list;
  int return_value, errnum, retval = 0;
  
  assert(funccreate && funcdestroy && funcname);
  
  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");
  
  if (genders_load_data(handle, filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
  
  return_value = funccreate(handle, &list);
  errnum = genders_errnum(handle);
  
  retval = genders_return_value_errnum_check(funcname,
					     num,
					     expected_return_value,
					     expected_errnum,
					     return_value,
					     errnum,
					     msg,
					     verbose);
  
  if (expected_return_value >= 0
      && expected_return_value == retval
      && errnum == GENDERS_ERR_SUCCESS)
    {
      if (funcdestroy(handle, list) < 0)
	genders_err_exit("funcdestroy: %s", genders_errormsg(handle));
    }
  
  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_create");
  
  return retval;
}

static int
_genders_list_clear_functionality(GendersListCreateFunc funccreate,
				  GendersListClearFunc funcclear,
				  GendersListDestroyFunc funcdestroy,
				  char *funcname,
				  char *filename,
				  int num,
				  int expected_return_value,
				  int expected_errnum,
				  char *msg,
				  int verbose)
{
  genders_t handle;
  char **list;
  int err, list_len, return_value, errnum, retval = 0;

  assert(funccreate && funcclear && funcdestroy && funcname);

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");
  
  if (genders_load_data(handle, filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));

  if ((list_len = funccreate(handle, &list)) < 0)
    genders_err_exit("funccreate: %s", genders_errormsg(handle));

  return_value = funcclear(handle, list);
  errnum = genders_errnum(handle);
  
  err = genders_return_value_errnum_list_check(funcname,
					       num,
					       0,
					       GENDERS_ERR_SUCCESS,
					       NULL, 
					       0,
					       return_value,
					       errnum,
					       list,
					       list_len,
					       GENDERS_COMPARISON_CLEAR,
					       msg,
					       verbose);

  if (funcdestroy(handle, list) < 0)
    genders_err_exit("funcdestroy: %s", genders_errormsg(handle));
  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_destroy: %s", genders_errormsg(handle));
  
  return retval;
}

static int
_genders_list_destroy_functionality(GendersListCreateFunc funccreate,
				    GendersListDestroyFunc funcdestroy,
				    char *funcname,
				    char *filename,
				    int num,
				    int expected_return_value,
				    int expected_errnum,
				    char *msg,
				    int verbose)
{
  genders_t handle;
  char **list;
  int return_value, errnum, retval = 0;

  assert(funccreate && funcdestroy && funcname);

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");
  
  if (genders_load_data(handle, filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));

  if (funccreate(handle, &list) < 0)
    genders_err_exit("funccreate: %s", genders_errormsg(handle));

  return_value = funcdestroy(handle, list);
  errnum = genders_errnum(handle);
      
  retval = genders_return_value_errnum_check(funcname,
					     num,
					     expected_return_value,
					     expected_errnum,
					     return_value,
					     errnum,
					     msg,
					     verbose);

  /* If funcdestroy failed, not need to try again, just ignore destroying the list*/
  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_destroy: %s", genders_errormsg(handle));

  return retval;
}

int
genders_nodelist_create_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_create_functionality(genders_nodelist_create,
					       genders_nodelist_destroy,
					       "genders_nodelist_create",
					       databases[i]->filename,
					       num, 
					       databases[i]->data->numnodes,
					       GENDERS_ERR_SUCCESS,
					       databases[i]->filename,
					       verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_nodelist_clear_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_clear_functionality(genders_nodelist_create,
					      genders_nodelist_clear,
					      genders_nodelist_destroy,
					      "genders_nodelist_clear",
					      databases[i]->filename,
					      num, 
					      0,
					      GENDERS_ERR_SUCCESS,
					      databases[i]->filename,
					      verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_nodelist_destroy_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_destroy_functionality(genders_nodelist_create,
						genders_nodelist_destroy,
						"genders_nodelist_destroy",
						databases[i]->filename,
						num, 
						0,
						GENDERS_ERR_SUCCESS,
						databases[i]->filename,
						verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_attrlist_create_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_create_functionality(genders_attrlist_create,
					       genders_attrlist_destroy,
					       "genders_attrlist_create",
					       databases[i]->filename,
					       num, 
					       databases[i]->data->numattrs,
					       GENDERS_ERR_SUCCESS,
					       databases[i]->filename,
					       verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_attrlist_clear_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_clear_functionality(genders_nodelist_create,
					      genders_nodelist_clear,
					      genders_nodelist_destroy,
					      "genders_nodelist_clear",
					      databases[i]->filename,
					      num, 
					      0,
					      GENDERS_ERR_SUCCESS,
					      databases[i]->filename,
					      verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_attrlist_destroy_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_destroy_functionality(genders_attrlist_create,
						genders_attrlist_destroy,
						"genders_attrlist_destroy",
						databases[i]->filename,
						num, 
						0,
						GENDERS_ERR_SUCCESS,
						databases[i]->filename,
						verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_vallist_create_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_create_functionality(genders_vallist_create,
					       genders_vallist_destroy,
					       "genders_vallist_create",
					       databases[i]->filename,
					       num, 
					       databases[i]->data->numattrs,
					       GENDERS_ERR_SUCCESS,
					       databases[i]->filename,
					       verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_vallist_clear_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_clear_functionality(genders_vallist_create,
					      genders_vallist_clear,
					      genders_vallist_destroy,
					      "genders_vallist_clear",
					      databases[i]->filename,
					      num, 
					      0,
					      GENDERS_ERR_SUCCESS,
					      databases[i]->filename,
					      verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_vallist_destroy_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int err;
      
      err = _genders_list_destroy_functionality(genders_vallist_create,
						genders_vallist_destroy,
						"genders_vallist_destroy",
						databases[i]->filename,
						num, 
						0,
						GENDERS_ERR_SUCCESS,
						databases[i]->filename,
						verbose);
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_getnodename_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];
  char nodenamebuf[MAXHOSTNAMELEN+1];
  char *ptr;

  memset(nodenamebuf, '\0', MAXHOSTNAMELEN+1);
  if (gethostname(nodenamebuf, MAXHOSTNAMELEN) < 0)
    genders_err_exit("gethostname: %s", strerror(errno));

  /* shorten if necessary */
  if ((ptr = strchr(nodenamebuf,'.')))
    *ptr = '\0';

  while (databases[i] != NULL)
    {
      char testbuf[MAXHOSTNAMELEN+1];
      int return_value, errnum, err;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
      
      return_value = genders_getnodename(handle, testbuf, MAXHOSTNAMELEN);
      errnum = genders_errnum(handle);
      
      err = genders_return_value_errnum_string_check("genders_getnodename",
						     num, 
						     0,
						     GENDERS_ERR_SUCCESS,
						     nodenamebuf,
						     return_value,
						     errnum,
						     testbuf,
						     databases[i]->filename,
						     verbose);
      
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_getnodes_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int j, nodelist_len, return_value, errnum, err;
      char **nodelist;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
      
      if ((nodelist_len = genders_nodelist_create(handle, &nodelist)) < 0) 
	genders_err_exit("genders_nodelist_create: %s", genders_errormsg(handle));
	  
      for (j = 0; j < databases[i]->data->attrval_nodes_len; j++)
	{
	  return_value = genders_getnodes(handle, 
					  nodelist,
					  nodelist_len,
					  databases[i]->data->attrval_nodes[j].attr,
					  databases[i]->data->attrval_nodes[j].val);
	  errnum = genders_errnum(handle);
	    
	  err = genders_return_value_errnum_list_check("genders_getnodes",
						       num,
						       databases[i]->data->attrval_nodes[j].nodeslen,
						       GENDERS_ERR_SUCCESS,
						       databases[i]->data->attrval_nodes[j].nodes,
						       databases[i]->data->attrval_nodes[j].nodeslen,
						       return_value,
						       errnum,
						       nodelist,
						       return_value,
						       GENDERS_COMPARISON_MATCH,
						       databases[i]->filename,
						       verbose);
	  
	  errcount += err;
	}

      /* Test invalid attr */
      return_value = genders_getnodes(handle,
				      nodelist,
				      nodelist_len,
				      GENDERS_DATABASE_INVALID_ATTR,
				      NULL);
      errnum = genders_errnum(handle);
      
      err = genders_return_value_errnum_check("genders_getnodes",
					      num,
					      0,
					      GENDERS_ERR_SUCCESS,
					      return_value,
					      errnum,
					      databases[i]->filename,
					      verbose);
      errcount += err;

      /* Test attr with val, but invalid val */
      if (databases[i]->data->attr_with_val)
	{
	  return_value = genders_getnodes(handle,
					  nodelist,
					  nodelist_len,
					  databases[i]->data->attr_with_val,
					  GENDERS_DATABASE_INVALID_VAL);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_getnodes",
						  num,
						  0,
						  GENDERS_ERR_SUCCESS,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}

      if (genders_nodelist_destroy(handle, nodelist) < 0)
	genders_err_exit("genders_nodelist_destroy: %s", genders_errormsg(handle));
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
      
      num++;
      i++;
    }

  return errcount;
}

int
genders_getattr_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int j, attrlist_len, vallist_len, return_value, errnum, err;
      char **attrlist, **vallist;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
      
      if ((attrlist_len = genders_attrlist_create(handle, &attrlist)) < 0) 
	genders_err_exit("genders_attrlist_create: %s", genders_errormsg(handle));
      
      if ((vallist_len = genders_vallist_create(handle, &vallist)) < 0) 
	genders_err_exit("genders_vallist_create: %s", genders_errormsg(handle));
      
      for (j = 0; j < databases[i]->data->nodeslen; j++)
	{
	  err = 0;
	  return_value = genders_getattr(handle, 
					 attrlist, 
					 vallist, 
					 attrlist_len, 
					 databases[i]->data->nodes[j]);
	  errnum = genders_errnum(handle);

	  err += genders_return_value_errnum_attrval_list_check("genders_getattr",
								num,
								databases[i]->data->node_attrvals[j].attrslen,
								GENDERS_ERR_SUCCESS,
								databases[i]->data->node_attrvals[j].attrs,
 								databases[i]->data->node_attrvals[j].vals_string,
								databases[i]->data->node_attrvals[j].attrslen,
								return_value,
								errnum,
								attrlist,
								vallist,
								return_value,
								databases[i]->filename,
								verbose);
	  errcount += err;
	}

      /* Test invalid node */
      if (databases[i]->data->node)
	{
	  return_value = genders_getattr(handle,
					 attrlist,
					 vallist,
					 attrlist_len,
					 GENDERS_DATABASE_INVALID_NODE);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_getattr",
						  num,
						  -1,
						  GENDERS_ERR_NOTFOUND,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}

      if (genders_attrlist_destroy(handle, attrlist) < 0)
	genders_err_exit("genders_attrlist_destroy: %s", genders_errormsg(handle));
      if (genders_vallist_destroy(handle, vallist) < 0)
	genders_err_exit("genders_vallist_destroy: %s", genders_errormsg(handle));
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
	
      num++;
      i++;
    }

  return errcount;
}

int
genders_getattr_all_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int attrlist_len, return_value, errnum, err;
      char **attrlist;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
      
      if ((attrlist_len = genders_attrlist_create(handle, &attrlist)) < 0) 
	genders_err_exit("genders_attrlist_create: %s", genders_errormsg(handle));
      
      return_value = genders_getattr_all(handle, attrlist, attrlist_len);
      errnum = genders_errnum(handle);
      
      err = genders_return_value_errnum_list_check("genders_getattr_all",
						   num,
						   databases[i]->data->numattrs,
						   GENDERS_ERR_SUCCESS,
						   databases[i]->data->attrs,
						   databases[i]->data->attrslen,
						   return_value,
						   errnum,
						   attrlist,
						   return_value,
						   GENDERS_COMPARISON_MATCH,
						   databases[i]->filename,
						   verbose);

      if (genders_attrlist_destroy(handle, attrlist) < 0)
	genders_err_exit("genders_attrlist_destroy: %s", genders_errormsg(handle));
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
      
      errcount += err;
      num++;
      i++;
    }

  return errcount;
}

int
genders_testattr_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];
  
  while (databases[i] != NULL)
    {
      int j, maxvallen, return_value, errnum, err;
      char *valbuf;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
      if ((maxvallen = genders_getmaxvallen(handle)) < 0)
	genders_err_exit("genders_getmaxvallen: %s", genders_errormsg(handle));
      if (!(valbuf = (char *)malloc(maxvallen + 1)))
	genders_err_exit("malloc: %s", strerror(errno));
      
      for (j = 0; j < databases[i]->data->nodeslen; j++)
	{
	  int k;
	  for (k = 0; k < databases[i]->data->node_attrvals[j].attrslen; k++)
	    {
	      return_value = genders_testattr(handle, 
					      databases[i]->data->nodes[j],
					      databases[i]->data->node_attrvals[j].attrs[k],
					      valbuf,
					      maxvallen + 1);
	      errnum = genders_errnum(handle);
	      
	      err = genders_return_value_errnum_string_check("genders_testattr",
							     num, 
							     1,
							     GENDERS_ERR_SUCCESS,
							     databases[i]->data->node_attrvals[j].vals_string[k],
							     return_value,
							     errnum,
							     valbuf,
							     databases[i]->filename,
							     verbose);
		
	      errcount += err;
	    }
	}
      
      /* Test invalid node */
      if (databases[i]->data->node && databases[i]->data->attr_with_val)
	{
	  return_value = genders_testattr(handle,
					  GENDERS_DATABASE_INVALID_NODE,
					  databases[i]->data->attr_with_val,
					  valbuf,
					  maxvallen + 1);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_testattr",
						  num,
						  -1,
						  GENDERS_ERR_NOTFOUND,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}

      /* Test invalidate attribute */
      return_value = genders_testattr(handle, 
				      databases[i]->data->node,
				      GENDERS_DATABASE_INVALID_ATTR,
				      valbuf,
				      maxvallen + 1);
      errnum = genders_errnum(handle);
      
      err = genders_return_value_errnum_check("genders_testattr",
					      num,
					      0,
					      GENDERS_ERR_SUCCESS,
					      return_value,
					      errnum,
					      databases[i]->filename,
					      verbose);
      errcount += err;

      free(valbuf);
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
      
      num++;
      i++;
    }

  return errcount;
}

int
genders_testattrval_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int j, return_value, errnum, err;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
      
      for (j = 0; j < databases[i]->data->nodeslen; j++)
	{
	  int k;
	  for (k = 0; k < databases[i]->data->node_attrvals[j].attrslen; k++)
	    {
	      return_value = genders_testattrval(handle, 
						 databases[i]->data->nodes[j],
						 databases[i]->data->node_attrvals[j].attrs[k],
						 databases[i]->data->node_attrvals[j].vals_input[k]);
	      errnum = genders_errnum(handle);
	      
	      err = genders_return_value_errnum_check("genders_testattrval",
						      num,
						      1,
						      GENDERS_ERR_SUCCESS,
						      return_value,
						      errnum,
						      databases[i]->filename,
						      verbose);
	      errcount += err;
	    }
	}
      
      /* Test invalid node */
      if (databases[i]->data->node && databases[i]->data->attr_with_val && databases[i]->data->val)
	{
	  return_value = genders_testattrval(handle,
					     GENDERS_DATABASE_INVALID_NODE,
					     databases[i]->data->attr_with_val,
					     databases[i]->data->val);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_testattrval",
						  num,
						  -1,
						  GENDERS_ERR_NOTFOUND,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}

      /* Test attr without val */
      if (databases[i]->data->attr_without_val && databases[i]->data->val)
	{
	  return_value = genders_testattrval(handle, 
					     databases[i]->data->node,
					     databases[i]->data->attr_without_val,
					     databases[i]->data->val);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_testattrval",
						  num,
						  0,
						  GENDERS_ERR_SUCCESS,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}
      
      /* Test attr with val, but invalid val */
      if (databases[i]->data->attr_with_val)
	{
	  return_value = genders_testattrval(handle, 
					     databases[i]->data->node,
					     databases[i]->data->attr_with_val,
					     GENDERS_DATABASE_INVALID_VAL);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_testattrval",
						  num,
						  0,
						  GENDERS_ERR_SUCCESS,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  
	  errcount += err;
	}

      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");

      num++;
      i++;
  }

  return errcount;
}

int
genders_isnode_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int j, return_value, errnum, err;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
      
      for (j = 0; j < databases[i]->data->nodeslen; j++)
	{
	  return_value = genders_isnode(handle, databases[i]->data->nodes[j]);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_isnode",
						  num,
						  1,
						  GENDERS_ERR_SUCCESS,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}
      
      /* Test invalid node */
      return_value = genders_isnode(handle, GENDERS_DATABASE_INVALID_NODE);
      errnum = genders_errnum(handle);
      
      err = genders_return_value_errnum_check("genders_isnode",
					      num,
					      0,
					      GENDERS_ERR_SUCCESS,
					      return_value,
					      errnum,
					      databases[i]->filename,
					      verbose);
      errcount += err;

      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
      
      num++;
      i++;
    }

  return errcount;
}

int
genders_isattr_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int j, return_value, errnum, err;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
	
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
      
      for (j = 0; j < databases[i]->data->attrslen; j++)
	{
	  return_value = genders_isattr(handle, databases[i]->data->attrs[j]);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_isattr",
						  num,
						  1,
						  GENDERS_ERR_SUCCESS,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}
      
      /* Test invalid attr */
      return_value = genders_isattr(handle, GENDERS_DATABASE_INVALID_ATTR);
      errnum = genders_errnum(handle);
      
      err = genders_return_value_errnum_check("genders_isattr",
					      num,
					      0,
					      GENDERS_ERR_SUCCESS,
					      return_value,
					      errnum,
					      databases[i]->filename,
					      verbose);
      errcount += err;
      
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
      
      num++;
      i++;
    }

  return errcount;
}

int
genders_isattrval_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int j, return_value, errnum, err;
      
      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
      for (j = 0; j < databases[i]->data->attrslen; j++)
	{
	  if (!databases[i]->data->vals[j])
	    continue;

	  return_value = genders_isattrval(handle, 
					   databases[i]->data->attrs[j],
					   databases[i]->data->vals[j]);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_isattrval",
						  num,
						  1,
						  GENDERS_ERR_SUCCESS,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}
      
      /* Test attr without val */
      if (databases[i]->data->attr_without_val && databases[i]->data->val)
	{
	  return_value = genders_isattrval(handle, 
					   databases[i]->data->attr_without_val,
					   databases[i]->data->val);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_isattrval",
						  num,
						  0,
						  GENDERS_ERR_SUCCESS,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}
      
      /* Test attr with val, but invalid val */
      if (databases[i]->data->attr_with_val)
	{
	  return_value = genders_isattrval(handle, 
					   databases[i]->data->attr_with_val,
					   GENDERS_DATABASE_INVALID_VAL);
	  errnum = genders_errnum(handle);
	  
	  err = genders_return_value_errnum_check("genders_isattrval",
						  num,
						  0,
						  GENDERS_ERR_SUCCESS,
						  return_value,
						  errnum,
						  databases[i]->filename,
						  verbose);
	  errcount += err;
	}
	  
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
	  
      num++;
      i++;
    }

  return errcount;
}

int
genders_index_attrvals_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i = 0;
  genders_database_t **databases = &genders_functionality_databases[0];

  while (databases[i] != NULL)
    {
      int j, return_value, errnum, err;

      /* Testing genders_index_attrvals will involve
       * A: running genders_index_attrvals()
       * B: re-running genders_getnodes() tests
       * C: re-running genders_isattrval() tests
       */

      if (!(handle = genders_handle_create()))
	genders_err_exit("genders_handle_create");
      
      if (genders_load_data(handle, databases[i]->filename) < 0)
	genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
      
      for (j = 0; j < databases[i]->data->attrslen; j++)
	{
	  err = 0;
	  return_value = genders_index_attrvals(handle, databases[i]->data->attrs[j]);
	  errnum = genders_errnum(handle);
      
	  err += genders_return_value_errnum_check("genders_index_attrvals",
						   num, 
						   0,
						   GENDERS_ERR_SUCCESS,
						   return_value,
						   errnum,
						   databases[i]->filename,
						   verbose);

	  /* genders_getnodes() tests */
	  if (err == 0)
	    {
	      int k;
	      char **nodelist;
	      int nodelist_len;

	      if ((nodelist_len = genders_nodelist_create(handle, &nodelist)) < 0) 
		genders_err_exit("genders_nodelist_create: %s", genders_errormsg(handle));
	  
	      for (k = 0; k < databases[i]->data->attrval_nodes_len; k++)
		{
		  return_value = genders_getnodes(handle, 
						  nodelist,
						  nodelist_len,
						  databases[i]->data->attrval_nodes[k].attr,
						  databases[i]->data->attrval_nodes[k].val);
		  errnum = genders_errnum(handle);
		  
		  err += genders_return_value_errnum_list_check("genders_index_attrvals:genders_getnodes",
								num,
								databases[i]->data->attrval_nodes[k].nodeslen,
								GENDERS_ERR_SUCCESS,
								databases[i]->data->attrval_nodes[k].nodes,
								databases[i]->data->attrval_nodes[k].nodeslen,
								return_value,
								errnum,
								nodelist,
								return_value,
								GENDERS_COMPARISON_MATCH,
								databases[i]->filename,
								verbose);
		}

	      /* Test invalid attr */
	      return_value = genders_getnodes(handle,
					      nodelist,
					      nodelist_len,
					      GENDERS_DATABASE_INVALID_ATTR,
					      NULL);
	      errnum = genders_errnum(handle);
	      
	      err += genders_return_value_errnum_check("genders_index_attrvals:genders_getnodes",
						       num,
						       0,
						       GENDERS_ERR_SUCCESS,
						       return_value,
						       errnum,
						       databases[i]->filename,
						       verbose);

	      /* Test attr with val, but invalid val */
	      if (databases[i]->data->attr_with_val)
		{
		  return_value = genders_getnodes(handle,
						  nodelist,
						  nodelist_len,
						  databases[i]->data->attr_with_val,
						  GENDERS_DATABASE_INVALID_VAL);
		  errnum = genders_errnum(handle);
		  
		  err += genders_return_value_errnum_check("genders_index_attrvals:genders_getnodes",
							   num,
							   0,
							   GENDERS_ERR_SUCCESS,
							   return_value,
							   errnum,
							   databases[i]->filename,
							   verbose);
		}

	      if (genders_nodelist_destroy(handle, nodelist) < 0)
		genders_err_exit("genders_nodelist_destroy: %s", genders_errormsg(handle));
	    }

	  /* genders_isattrval() tests */
	  if (err == 0)
	    {
	      int k;
	      for (k = 0; k < databases[i]->data->attrslen; k++)
		{
		  if (!databases[i]->data->vals[k])
		    continue;
		  
		  return_value = genders_isattrval(handle, 
						   databases[i]->data->attrs[k],
						   databases[i]->data->vals[k]);
		  errnum = genders_errnum(handle);
		  
		  err += genders_return_value_errnum_check("genders_index_attrvals:genders_isattrval",
							   num,
							   1,
							   GENDERS_ERR_SUCCESS,
							   return_value,
							   errnum,
							   databases[i]->filename,
							   verbose);
		}
	      
	      /* Test attr without val */
	      if (databases[i]->data->attr_without_val && databases[i]->data->val)
		{
		  return_value = genders_isattrval(handle, 
						   databases[i]->data->attr_without_val,
						   databases[i]->data->val);
		  errnum = genders_errnum(handle);
		  
		  err += genders_return_value_errnum_check("genders_index_attrvals:genders_isattrval",
							   num,
							   0,
							   GENDERS_ERR_SUCCESS,
							   return_value,
							   errnum,
							   databases[i]->filename,
							   verbose);
		}
      
	      /* Test attr with val, but invalid val */
	      if (databases[i]->data->attr_with_val)
		{
		  return_value = genders_isattrval(handle, 
						   databases[i]->data->attr_with_val,
						   GENDERS_DATABASE_INVALID_VAL);
		  errnum = genders_errnum(handle);
		  
		  err += genders_return_value_errnum_check("genders_index_attrvals:genders_isattrval",
							   num,
							   0,
							   GENDERS_ERR_SUCCESS,
							   return_value,
							   errnum,
							   databases[i]->filename,
							   verbose);
		}
	    }
	  
	  errcount += err;
	}
      
      if (genders_handle_destroy(handle) < 0)
	genders_err_exit("genders_handle_destroy");
      
      num++;
      i++;
    }

  return errcount;
}

int
genders_query_functionality(int verbose)
{
  char msgbuf[GENDERS_ERR_BUFLEN];
  int errcount = 0;
  int num = 0;

  /* Part A: Parse error queries */
  {
    genders_t handle;
    int nodelist_len, return_value, errnum, err;
    char **nodelist;
    int i = 0;
      
    if (!(handle = genders_handle_create()))
      genders_err_exit("genders_handle_create");
	
    if (genders_load_data(handle, genders_database_base.filename) < 0)
      genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
    if ((nodelist_len = genders_nodelist_create(handle, &nodelist)) < 0) 
      genders_err_exit("genders_nodelist_create: %s", genders_errormsg(handle));
	
    while (genders_query_parse_error_tests[i] != NULL)
      {
	return_value = genders_query(handle,
				     nodelist,
				     nodelist_len,
				     genders_query_parse_error_tests[i]);
	errnum = genders_errnum(handle);
	
	sprintf(msgbuf, "\"%s\"", genders_query_parse_error_tests[i]);
	err = genders_return_value_errnum_check("genders_query",
						num,
						-1,
						GENDERS_ERR_SYNTAX,
						return_value,
						errnum,
						msgbuf,
						verbose);
	errcount += err;
	num++;
	i++;
      }

    if (genders_nodelist_destroy(handle, nodelist) < 0)
      genders_err_exit("genders_nodelist_destroy: %s", genders_errormsg(handle));
    if (genders_handle_destroy(handle) < 0)
      genders_err_exit("genders_handle_destroy");
  }

  /* Part B: Simple attr and attr=val queries  */
  {
    int i = 0;
    genders_t handle;
    genders_database_t **databases = &genders_functionality_databases[0];
    while (databases[i] != NULL)
      {
	int j, nodelist_len, return_value, errnum, err;
	char **nodelist;
      
	if (!(handle = genders_handle_create()))
	  genders_err_exit("genders_handle_create");
	
	if (genders_load_data(handle, databases[i]->filename) < 0)
	  genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
	if ((nodelist_len = genders_nodelist_create(handle, &nodelist)) < 0) 
	  genders_err_exit("genders_nodelist_create: %s", genders_errormsg(handle));
	
	for (j = 0; j < databases[i]->data->attrval_nodes_len; j++)
	  {
	    char querybuf[GENDERS_QUERY_BUFLEN];
	    char *queryptr;
	    
	    /* Construct simple query */
	    if (databases[i]->data->attrval_nodes[j].attr)
	      {
		memset(querybuf, '\0', GENDERS_QUERY_BUFLEN);
		strcpy(querybuf, databases[i]->data->attrval_nodes[j].attr);
		if (databases[i]->data->attrval_nodes[j].val)
		  {
		    strcat(querybuf, "=");
		    strcat(querybuf, databases[i]->data->attrval_nodes[j].val);
		  }
		queryptr = querybuf;
	      }
	    else
	      queryptr = NULL;
	    
	    
	    return_value = genders_query(handle, 
					 nodelist,
					 nodelist_len,
					 queryptr);
	    errnum = genders_errnum(handle);
	    
	    err = genders_return_value_errnum_list_check("genders_query",
							 num,
							 databases[i]->data->attrval_nodes[j].nodeslen,
							 GENDERS_ERR_SUCCESS,
							 databases[i]->data->attrval_nodes[j].nodes,
							 databases[i]->data->attrval_nodes[j].nodeslen,
							 return_value,
							 errnum,
							 nodelist,
							 return_value,
							 GENDERS_COMPARISON_MATCH,
							 databases[i]->filename,
							 verbose);
	    
	    errcount += err;
	  }
	
	/* Test invalid attr */
	return_value = genders_query(handle, 
				     nodelist,
				     nodelist_len,
				     GENDERS_DATABASE_INVALID_ATTR);
	errnum = genders_errnum(handle);
	
	err = genders_return_value_errnum_check("genders_query",
						num,
						0,
						GENDERS_ERR_SUCCESS,
						return_value,
						errnum,
						databases[i]->filename,
						verbose);
	errcount += err;

	/* Test attr with val, but invalid val */
	if (databases[i]->data->attr_with_val)
	  {
	    char querybuf[GENDERS_QUERY_BUFLEN];
	    
	    memset(querybuf, '\0', GENDERS_QUERY_BUFLEN);
	    strcpy(querybuf, databases[i]->data->attr_with_val);
	    strcat(querybuf, "=");
	    strcat(querybuf, GENDERS_DATABASE_INVALID_VAL);

	    return_value = genders_query(handle,
					 nodelist,
					 nodelist_len,
					 querybuf);
	    errnum = genders_errnum(handle);
	  
	    err = genders_return_value_errnum_check("genders_query",
						    num,
						    0,
						    GENDERS_ERR_SUCCESS,
						    return_value,
						    errnum,
						    databases[i]->filename,
						    verbose);
	    errcount += err;
	  }

	if (genders_nodelist_destroy(handle, nodelist) < 0)
	  genders_err_exit("genders_nodelist_destroy: %s", genders_errormsg(handle));
	if (genders_handle_destroy(handle) < 0)
	  genders_err_exit("genders_handle_destroy");
	
	num++;
	i++;
      }
  }

  /* Part C: Complex queries  */
  {
    int i = 0;
    genders_t handle;
    genders_query_functionality_tests_t **databases = &genders_query_functionality_tests[0];

    while (databases[i] != NULL)
      {
	int j, nodelist_len, return_value, errnum, err;
	char **nodelist;
      
	if (!(handle = genders_handle_create()))
	  genders_err_exit("genders_handle_create");
	
	if (genders_load_data(handle, databases[i]->filename) < 0)
	  genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
	if ((nodelist_len = genders_nodelist_create(handle, &nodelist)) < 0) 
	  genders_err_exit("genders_nodelist_create: %s", genders_errormsg(handle));
	
	j = 0;
	while (databases[i]->tests->tests[j].query != NULL)
	  {
	    return_value = genders_query(handle, 
					 nodelist,
					 nodelist_len,
					 databases[i]->tests->tests[j].query);
	    errnum = genders_errnum(handle);

	    sprintf(msgbuf, "%s: \"%s\"", 
		    databases[i]->filename,
		    databases[i]->tests->tests[j].query);
	    err = genders_return_value_errnum_list_check("genders_query",
							 num,
							 databases[i]->tests->tests[j].nodeslen,
							 GENDERS_ERR_SUCCESS,
							 databases[i]->tests->tests[j].nodes,
							 databases[i]->tests->tests[j].nodeslen,
							 return_value,
							 errnum,
							 nodelist,
							 return_value,
							 GENDERS_COMPARISON_MATCH,
							 msgbuf,
							 verbose);
	    
	    errcount += err;
	    j++;
	  }

	if (genders_nodelist_destroy(handle, nodelist) < 0)
	  genders_err_exit("genders_nodelist_destroy: %s", genders_errormsg(handle));
	if (genders_handle_destroy(handle) < 0)
	  genders_err_exit("genders_handle_destroy");
	
	num++;
	i++;
      }
  }

  return errcount;
}

int
genders_parse_functionality(int verbose)
{
  int errcount = 0;
  int num = 0;
  FILE *stderr_save;
  FILE *dev_null;

  stderr_save = stderr;
  if (!(dev_null = fopen(_PATH_DEVNULL, "r+")))
    genders_err_exit("fopen: %s: %s", _PATH_DEVNULL, strerror(errno));
  
  /* Part A: Successfully find parse errors */
  {
    int i = 0;
    genders_parse_error_database_t *databases = &genders_parse_error_databases[0];

    while (databases[i].filename != NULL)
      {
	genders_t handle;
	int err, return_value, errnum;

	if (!(handle = genders_handle_create()))
	  genders_err_exit("genders_handle_create");

	/* Route stderr somewhere else during call to genders_parse() */
	stderr = dev_null;
	return_value = genders_parse(handle, databases[i].filename, NULL);
	stderr = stderr_save;
	errnum = genders_errnum(handle);

	err = genders_return_value_errnum_check("genders_parse",
						num,
						databases[i].expected_parse_errors,
						GENDERS_ERR_SUCCESS,
						return_value,
						errnum,
						databases[i].filename,
						verbose);

	if (genders_handle_destroy(handle) < 0)
	  genders_err_exit("genders_handle_create");

	errcount += err;
	num++;
	i++;
      }
  }

  /* Part B: Load functionality test databases without errors */
  {
    int i = 0;
    genders_database_t **databases = &genders_functionality_databases[0];

    while (databases[i] != NULL)
      {
	genders_t handle;
	int err, return_value, errnum;

	if (!(handle = genders_handle_create()))
	  genders_err_exit("genders_handle_create");

	/* Route stderr somewhere else during call to genders_parse() */
	stderr = dev_null;
	return_value = genders_parse(handle, databases[i]->filename, NULL);
	stderr = stderr_save;
	errnum = genders_errnum(handle);

	err = genders_return_value_errnum_check("genders_parse",
						num,
						0,
						GENDERS_ERR_SUCCESS,
						return_value,
						errnum,
						databases[i]->filename,
						verbose);

	if (genders_handle_destroy(handle) < 0)
	  genders_err_exit("genders_handle_create");

	errcount += err;
	num++;
	i++;
      }
  }

  fclose(dev_null);
  return errcount;
}

int
genders_set_errnum_functionality(int verbose)
{
  genders_t handle;
  int errcount = 0;
  int num = 0;
  int i;

  if (!(handle = genders_handle_create()))
    genders_err_exit("genders_handle_create");
  
  if (genders_load_data(handle, genders_database_base.filename) < 0)
    genders_err_exit("genders_load_data: %s", genders_errormsg(handle));
	
  for (i = GENDERS_ERR_SUCCESS; i <= GENDERS_ERR_ERRNUMRANGE; i++)
    {
      int errnum, err;

      genders_set_errnum(handle, i);
      errnum = genders_errnum(handle);
	
      err = genders_errnum_check("genders_set_errnum",
				 num,
				 i,
				 errnum,
				 NULL,
				 verbose);

      errcount += err;
      num++;
    }

  if (genders_handle_destroy(handle) < 0)
    genders_err_exit("genders_handle_create");

  return errcount;
}