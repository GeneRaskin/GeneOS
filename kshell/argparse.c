/* Code by cofys
 * https://github.com/cofyc/argparse */

#include "../cstd/include/libft.h"
#include "../debug/errno.h"
#include "argparse.h"

#define OPT_UNSET 1
#define OPT_LONG  (1 << 1)

static const char *
prefix_skip(const char *str, const char *prefix)
{
    size_t len = ft_strlen(prefix);
    return ft_strncmp(str, prefix, len) ? NULL : str + len;
}

static int
prefix_cmp(const char *str, const char *prefix)
{
    for (;; str++, prefix++)
        if (!*prefix) {
            return 0;
        } else if (*str != *prefix) {
            return (unsigned char)*prefix - (unsigned char)*str;
        }
}

static int
argparse_error(struct argparse *self, const struct argparse_option *opt,
               const char *reason, int flags)
{
    (void)self;
    if (flags & OPT_LONG) {
        ft_printf("error: option `--%s` %s\n", opt->long_name, reason);
    } else {
        ft_printf("error: option `-%c` %s\n", opt->short_name, reason);
    }
    return (1);
}

static int
argparse_getvalue(struct argparse *self, const struct argparse_option *opt,
                  int flags)
{
    const char *s = NULL;
    if (!opt->value)
        goto skipped;
    switch (opt->type) {
        case ARGPARSE_OPT_BOOLEAN:
            if (flags & OPT_UNSET) {
                *(int *)opt->value = *(int *)opt->value - 1;
            } else {
                *(int *)opt->value = *(int *)opt->value + 1;
            }
            if (*(int *)opt->value < 0) {
                *(int *)opt->value = 0;
            }
            break;
        case ARGPARSE_OPT_BIT:
            if (flags & OPT_UNSET) {
                *(int *)opt->value &= ~opt->data;
            } else {
                *(int *)opt->value |= opt->data;
            }
            break;
        case ARGPARSE_OPT_STRING:
            if (self->optvalue) {
                *(const char **)opt->value = self->optvalue;
                self->optvalue             = NULL;
            } else if (self->argc > 1) {
                self->argc--;
                *(const char **)opt->value = *++self->argv;
            } else {
                return (argparse_error(self, opt, "requires a value", flags));
            }
            break;
        case ARGPARSE_OPT_INTEGER:
            errno = 0;
            if (self->optvalue) {
                *(int *)opt->value = ft_strtol(self->optvalue, (char **)&s, 0);
                self->optvalue     = NULL;
            } else if (self->argc > 1) {
                self->argc--;
                *(int *)opt->value = ft_strtol(*++self->argv, (char **)&s, 0);
            } else {
                return (argparse_error(self, opt, "requires a value", flags));
            }
            if (errno == ERANGE)
                return (argparse_error(self, opt, "numerical result out of range", flags));
            if (s[0] != '\0') // no digits or contains invalid characters
                return (argparse_error(self, opt, "expects an integer value", flags));
            break;
        case ARGPARSE_OPT_FLOAT:
            errno = 0;
            if (self->optvalue) {
                *(float *)opt->value = ft_strtof(self->optvalue, (char **)&s);
                self->optvalue       = NULL;
            } else if (self->argc > 1) {
                self->argc--;
                *(float *)opt->value = ft_strtof(*++self->argv, (char **)&s);
            } else {
                return (argparse_error(self, opt, "requires a value", flags));
            }
            if (errno == ERANGE)
                return (argparse_error(self, opt, "numerical result out of range", flags));
            if (s[0] != '\0') // no digits or contains invalid characters
                return (argparse_error(self, opt, "expects a numerical value", flags));
            break;
        default:
    }

    skipped:
    if (opt->callback) {
        return opt->callback(self, opt);
    }
    return 0;
}

static void
argparse_options_check(const struct argparse_option *options)
{
    for (; options->type != ARGPARSE_OPT_END; options++) {
        switch (options->type) {
            case ARGPARSE_OPT_END:
            case ARGPARSE_OPT_BOOLEAN:
            case ARGPARSE_OPT_BIT:
            case ARGPARSE_OPT_INTEGER:
            case ARGPARSE_OPT_FLOAT:
            case ARGPARSE_OPT_STRING:
            case ARGPARSE_OPT_GROUP:
                continue;
            default:
                ft_printf("wrong option type: %d", options->type);
                break;
        }
    }
}

static int
argparse_short_opt(struct argparse *self, const struct argparse_option *options)
{
    for (; options->type != ARGPARSE_OPT_END; options++) {
        if (options->short_name == *self->optvalue) {
            self->optvalue = self->optvalue[1] ? self->optvalue + 1 : NULL;
            return argparse_getvalue(self, options, 0);
        }
    }
    return -2;
}

static int
argparse_long_opt(struct argparse *self, const struct argparse_option *options)
{
    for (; options->type != ARGPARSE_OPT_END; options++) {
        const char *rest;
        int opt_flags = 0;
        if (!options->long_name)
            continue;

        rest = prefix_skip(self->argv[0] + 2, options->long_name);
        if (!rest) {
            // negation disabled?
            if (options->flags & OPT_NONEG) {
                continue;
            }
            // only OPT_BOOLEAN/OPT_BIT supports negation
            if (options->type != ARGPARSE_OPT_BOOLEAN && options->type !=
                                                         ARGPARSE_OPT_BIT) {
                continue;
            }

            if (prefix_cmp(self->argv[0] + 2, "no-")) {
                continue;
            }
            rest = prefix_skip(self->argv[0] + 2 + 3, options->long_name);
            if (!rest)
                continue;
            opt_flags |= OPT_UNSET;
        }
        if (*rest) {
            if (*rest != '=')
                continue;
            self->optvalue = rest + 1;
        }
        return argparse_getvalue(self, options, opt_flags | OPT_LONG);
    }
    return -2;
}

int
argparse_init(struct argparse *self, struct argparse_option *options,
              const char *const *usages, int flags)
{
    ft_memset(self, 0, sizeof(*self));
    self->options     = options;
    self->usages      = usages;
    self->flags       = flags;
    self->description = NULL;
    self->epilog      = NULL;
    return 0;
}

void
argparse_describe(struct argparse *self, const char *description,
                  const char *epilog)
{
    self->description = description;
    self->epilog      = epilog;
}

int
argparse_parse(struct argparse *self, int argc, const char **argv)
{
    self->argc = argc - 1;
    self->argv = argv + 1;
    self->out  = argv;

    argparse_options_check(self->options);

    for (; self->argc; self->argc--, self->argv++) {
        const char *arg = self->argv[0];
        if (arg[0] != '-' || !arg[1]) {
            if (self->flags & ARGPARSE_STOP_AT_NON_OPTION) {
                ft_printf("options without flags not allowed\n");
                return (argparse_help_cb_no_exit(self, self->options));
                goto end;
            }
            // if it's not option or is a single char '-', copy verbatim
            self->out[self->cpidx++] = self->argv[0];
            continue;
        }
        // short option
        if (arg[1] != '-') {
            self->optvalue = arg + 1;
            switch (argparse_short_opt(self, self->options)) {
                case 1:
                    return 1;
                case -1:
                    break;
                case -2:
                    goto unknown;
            }
            while (self->optvalue) {
                switch (argparse_short_opt(self, self->options)) {
                    case 1:
                        return 1;
                    case -1:
                        break;
                    case -2:
                        goto unknown;
                }
            }
            continue;
        }
        // if '--' presents
        if (!arg[2]) {
            self->argc--;
            self->argv++;
            break;
        }
        // long option
        switch (argparse_long_opt(self, self->options)) {
            case 1:
                return 1;
            case -1:
                break;
            case -2:
                goto unknown;
        }
        continue;

        unknown:
        ft_printf("error: unknown option `%s`\n", self->argv[0]);
        argparse_usage(self);
        if (!(self->flags & ARGPARSE_IGNORE_UNKNOWN_ARGS)) {
            return (1);
        }
    }

    end:
    ft_memmove(self->out + self->cpidx, self->argv,
            self->argc * sizeof(*self->out));
    self->out[self->cpidx + self->argc] = NULL;

    return self->cpidx + self->argc;
}

void
argparse_usage(struct argparse *self)
{
    if (self->usages) {
        ft_printf("Usage: %s\n", *self->usages++);
        while (*self->usages && **self->usages)
            ft_printf("   or: %s\n", *self->usages++);
    } else {
        ft_printf("Usage:\n");
    }

    // print description
    if (self->description)
        ft_printf("%s\n", self->description);

    ft_printf("\n");

    const struct argparse_option *options;

    // figure out best width
    size_t usage_opts_width = 0;
    size_t len;
    options = self->options;
    for (; options->type != ARGPARSE_OPT_END; options++) {
        len = 0;
        if ((options)->short_name) {
            len += 2;
        }
        if ((options)->short_name && (options)->long_name) {
            len += 2;           // separator ", "
        }
        if ((options)->long_name) {
            len += ft_strlen((options)->long_name) + 2;
        }
        if (options->type == ARGPARSE_OPT_INTEGER) {
            len += ft_strlen("=<int>");
        }
        if (options->type == ARGPARSE_OPT_FLOAT) {
            len += ft_strlen("=<flt>");
        } else if (options->type == ARGPARSE_OPT_STRING) {
            len += ft_strlen("=<str>");
        }
        len = (len + 3) - ((len + 3) & 3);
        if (usage_opts_width < len) {
            usage_opts_width = len;
        }
    }
    usage_opts_width += 4;      // 4 spaces prefix

    options = self->options;
    for (; options->type != ARGPARSE_OPT_END; options++) {
        size_t pos = 0;
        size_t pad = 0;
        if (options->type == ARGPARSE_OPT_GROUP) {
            ft_printf("\n");
            ft_printf("%s", options->help);
            ft_printf("\n");
            continue;
        }
        pos = ft_printf("    ");
        if (options->short_name) {
            pos += ft_printf("-%c", options->short_name);
        }
        if (options->long_name && options->short_name) {
            pos += ft_printf(", ");
        }
        if (options->long_name) {
            pos += ft_printf("--%s", options->long_name);
        }
        if (options->type == ARGPARSE_OPT_INTEGER) {
            pos += ft_printf("=<int>");
        } else if (options->type == ARGPARSE_OPT_FLOAT) {
            pos += ft_printf("=<flt>");
        } else if (options->type == ARGPARSE_OPT_STRING) {
            pos += ft_printf("=<str>");
        }
        if (pos <= usage_opts_width) {
            pad = usage_opts_width - pos;
        } else {
            ft_printf("\n");
            pad = usage_opts_width;
        }
        ft_printf("%*s%s\n", (int)pad + 2, "", options->help);
    }

    // print epilog
    if (self->epilog)
        ft_printf("%s\n", self->epilog);
}

int
argparse_help_cb_no_exit(struct argparse *self,
                         const struct argparse_option *option)
{
    (void)option;
    argparse_usage(self);
    return (1);
}

int
argparse_help_cb(struct argparse *self, const struct argparse_option *option)
{
    argparse_help_cb_no_exit(self, option);
    return (1);
}