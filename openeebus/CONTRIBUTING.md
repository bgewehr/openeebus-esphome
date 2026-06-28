# Contributing to OpenEEBUS by NIBE

> ❗ **Important – NIBE Product Support**
> 
> This forum is <ins>not</ins> for support requests or error reports related to NIBE products or the usage of EEBUS within NIBE products.
> Such inquiries will not be handled here. Please contact NIBE Customer Support through the official support channels for further help.

Thanks for helping make OpenEEBUS by NIBE better! ❤️

Every contribution matters and is valued, whether it’s a bug fix, a new feature idea, or a question that sparks discussion. You’ll find details below on the many ways you can help, along with guidelines to make the process smooth for everyone. Please read the relevant section before contributing, it makes life easier for maintainers and ensures a great experience for all involved. The community is excited to see what you bring!

## Discussions and Questions

Got an idea, a question, or a feature request? Start a new discussion in the OpenEEBUS repository under the [Discussions](https://github.com/NIBEGroup/openeebus/discussions) tab.

Before posting:

* Search existing discussions to see if your topic has already been addressed.
* If you find something similar but still need clarity, feel free to comment on that thread.
* A quick web search can also help you find solutions before posting.

If you still need to ask:

* Open a discussion.
* Provide as much background and detail as possible about the situation.

## Bug Reports

### Before You Report a Bug

A great bug report saves time and speeds up a fix. Please:

* Confirm you’re on the latest version.
* Verify it’s truly a bug, not a configuration or usage issue.
* Check the bug tracker to see if it’s already reported or solved.
* Gather details:
  * Stack trace
  * Relevant trace logs (SHIP/SPINE communication)
  * Input/output examples
  * Steps to reproduce (and whether it happens on older versions)

### Submitting a Bug Report

> Never post security vulnerabilities as public issues. Please see our [Vulnerability Disclosure Policy](SECURITY.md) for more information on how to report security issues.

We use GitHub issues for tracking. To report:

1. Open a new Issue without labeling it as a “bug” just yet.
1. Clearly describe the expected behavior vs. actual behavior.
1. Include precise reproduction steps and minimal code examples that isolate the problem.

## New Feature Requests
    
### Before You Suggest an Enhancement

* Make sure you’re on the latest version.
* Double-check the API to ensure it’s not already supported.
* Search Discussions for similar suggestions. If found, join the conversation instead of starting a new one.
* Consider whether the feature fits the project’s goals. We prioritize features useful to the majority.

### How to Propose It

Enhancement suggestions are tracked as Discussions:

* Use a clear, descriptive title.
* Explain your idea in detail—current behavior, desired behavior, and why it matters.
* Mention any alternatives you’ve tried that didn’t work.
* If relevant, link to other projects that do it well.

## Issue Tracker

The Issue Tracker is for bugs and agreed-upon improvements from prior discussions. Include:

* A descriptive title.
* The intent of the change.
* A link to the relevant discussion (if any).
* A detailed, step-by-step plan for the change.

## Pull Requests

> **Legal Notice**
> 
> By contributing, you confirm you wrote 100% of the content, hold the rights to it, and agree it may be provided under the project license.

We recommend starting pull requests (PRs) as drafts and committing early and often, this allows feedback before significant work is done. If your PR adds or changes functionality, discuss it in Discussions first. Only open an issue after positive community feedback.

In your PR:

* Reference the issue number being addressed.
* Describe the change and expected behavior.
* Add context where helpful.
* Make sure your code follows the style convention used by the project. The included `.clang-format` file can be used to automatically format your code.
* Ensure your code is well-tested. If you add new functionality, include unit tests that cover it.
* Ensure all checks pass, or explain any expected failures.

*Note:* Cosmetic changes (e.g., whitespace fixes) that don’t impact stability, functionality, or testability are generally not accepted.
